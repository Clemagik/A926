#include <linux/clk.h>
#include <linux/media-bus-format.h>
#include <linux/media.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_graph.h>
#include <linux/of_platform.h>
#include <linux/pm_runtime.h>
#include <linux/pm_domain.h>
#include <linux/slab.h>
#include <linux/videodev2.h>
#include <linux/component.h>
#include <linux/time.h>
#include <linux/delay.h>

#include <media/media-device.h>
#include <media/v4l2-async.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-event.h>
#include <media/videobuf2-dma-contig-ingenic.h>
#include <media/videobuf2-v4l2.h>

#include "isp-drv.h"
#include "mscaler-regs.h"

static void dump_mscaler_regs(struct mscaler_device *mscaler);

static struct isp_video_format mscaler_output_formats[] = {
	{
		.name     = "NV12, Y/CbCr 4:2:0",
		.fourcc   = V4L2_PIX_FMT_NV12,
		.depth    = {8, 4},
		.num_planes = 2,
		.mbus_code = MEDIA_BUS_FMT_YUYV8_2X8,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
	{
		.name     = "NV21, Y/CrCb 4:2:0",
		.fourcc   = V4L2_PIX_FMT_NV21,
		.depth    = {8, 4},
		.num_planes = 2,
		.mbus_code = MEDIA_BUS_FMT_YVYU8_2X8,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
	{
		.name     = "RGB565, RGB-5-6-5",
		.fourcc   = V4L2_PIX_FMT_RGB565,
		.depth    = {16},
		.num_planes = 1,
		.mbus_code = MEDIA_BUS_FMT_RGB565_1X16,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
	{
		.name     = "ARGB32, ARGB-8-8-8-8",
		.fourcc   = V4L2_PIX_FMT_ARGB32,
		.depth    = {32},
		.num_planes = 1,
		.mbus_code = MEDIA_BUS_FMT_ARGB8888_1X32,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
};


static inline void mscaler_reg_writel(struct mscaler_device *mscaler, unsigned int reg, unsigned int val)
{
	writel(val, mscaler->iobase + reg);
}

static inline unsigned int mscaler_reg_readl(struct mscaler_device *mscaler, unsigned int reg)
{
	return readl(mscaler->iobase + reg);
}


/* isp video 回调函数，用来寻找当前支持的输出视频格式，由于mscaler可以输出，vic也可以输出，所以这里设计成回调的方式.*/

/**
 * @brief
 *
 * @param pixelformat	如果pixelformat不为空，则使用pixelformat匹配.
 * @param mbus_code
 * @param index
 *
 * @return
 */
struct isp_video_format *mscaler_find_format(const u32 *pixelformat, const u32 *mbus_code, int index)
{
	int i;
	struct isp_video_format *fmt = NULL;

	for(i = 0; i < ARRAY_SIZE(mscaler_output_formats); i++) {
		fmt = &mscaler_output_formats[i];

		if(pixelformat && fmt->fourcc == *pixelformat) {
			return fmt;
		}
		if(mbus_code && fmt->mbus_code == *mbus_code) {
			return fmt;
		}

		if(index == i) {
			return fmt;
		}
	}

	return NULL;
}


/* In irq context. */
static int mscaler_irq_notify_ch_done(struct mscaler_device *mscaler, int ch)
{
	struct isp_video_buffer *isp_buffer = NULL;
	unsigned int y_last_addr = 0;
	unsigned int uv_last_addr = 0;
	unsigned int y_frame_num = 0;
	unsigned int uv_frame_num = 0;

	//printk("------%s, ch: %d\n", __func__, ch);


	spin_lock(&mscaler->lock);
	/*由于mscaler stop stream时, ISP 可能还会产生中断,这个时候不用处理*/
	if(mscaler->state[ch] == 0) {
		spin_unlock(&mscaler->lock);
		return 0;
	}

	isp_buffer = list_first_entry_or_null(&mscaler->dma_queued_list[ch], struct isp_video_buffer, list_entry);
	if(!isp_buffer) {
		dev_err(mscaler->dev, "[warning] no isp_buffer found in dma_queued_list when interrupt happend!\n");
		spin_unlock(&mscaler->lock);
		return 0;
	}

	list_del(&isp_buffer->list_entry);


	y_last_addr = mscaler_reg_readl(mscaler, CHx_DMAOUT_Y_LAST_ADDR(ch));
	y_frame_num = mscaler_reg_readl(mscaler, CHx_DMAOUT_Y_LAST_STATS_NUM(ch));

	uv_last_addr = mscaler_reg_readl(mscaler, CHx_DMAOUT_UV_LAST_ADDR(ch));
	uv_frame_num = mscaler_reg_readl(mscaler, CHx_DMAOUT_UV_LAST_STATS_NUM(ch));

	/*TODO Sequence.*/
	//if(isp_buffer->vb2.vb2_buf.state == VB2_BUF_STATE_ACTIVE)
	v4l2_get_timestamp(&isp_buffer->vb2.timestamp);
	isp_buffer->vb2.sequence = y_frame_num;

	vb2_buffer_done(&isp_buffer->vb2.vb2_buf, VB2_BUF_STATE_DONE);

	spin_unlock(&mscaler->lock);

	return 0;
}

static int mscaler_interrupt_service_routine(struct v4l2_subdev *sd,
						u32 status, bool *handled)
{
	struct mscaler_device *mscaler = v4l2_get_subdevdata(sd);
	int ret = 0;

	/*只管心 mscaler的三个ch的done中断和crop出错的中断.*/
	if(status & (1 << MSCA_CH0_FRM_DONE_INT)) {
		ret = mscaler_irq_notify_ch_done(mscaler, 0);
	}
	if(status & (1 << MSCA_CH1_FRM_DONE_INT)) {
		ret = mscaler_irq_notify_ch_done(mscaler, 1);
	}

	if(status & (1 << MSCA_CH2_FRM_DONE_INT)) {
		ret = mscaler_irq_notify_ch_done(mscaler, 2);
	}

	/*TODO: add handler.*/
	if(status & (1 << MSCA_CH0_CROP_ERR_INT)) {

	}
	if(status & (1 << MSCA_CH1_CROP_ERR_INT)) {

	}
	if(status & (1 << MSCA_CH2_CROP_ERR_INT)) {

	}

	return ret;
}

static int mscaler_subdev_init(struct v4l2_subdev *sd, u32 val)
{

	return 0;
}

static int mscaler_subdev_reset(struct v4l2_subdev *sd, u32 val)
{
	return 0;
}

static const struct v4l2_subdev_core_ops mscaler_subdev_core_ops = {
	.init	= mscaler_subdev_init,
	.reset = mscaler_subdev_reset,
        .log_status = v4l2_ctrl_subdev_log_status,
        .subscribe_event = v4l2_ctrl_subdev_subscribe_event,
        .unsubscribe_event = v4l2_event_subdev_unsubscribe,
	.interrupt_service_routine = mscaler_interrupt_service_routine,
};

static int mscaler_enum_mbus_code(struct v4l2_subdev *sd,
			      struct v4l2_subdev_pad_config *cfg,
			      struct v4l2_subdev_mbus_code_enum *code)
{
	//printk("------%s, %d, sd->name: %s\n", __func__, __LINE__, sd->name);

	if(code->index >= ARRAY_SIZE(mscaler_output_formats)) {
		dev_err(sd->dev, "too many mbus formats!\n");
		return -EINVAL;
	}

	code->code = mscaler_output_formats[code->index].mbus_code;

	return 0;
}
static int mscaler_enum_frame_size(struct v4l2_subdev *sd,
			       struct v4l2_subdev_pad_config *cfg,
			       struct v4l2_subdev_frame_size_enum *fse)
{
	//printk("------%s, %d, sd->name: %s\n", __func__, __LINE__, sd->name);
	fse->min_width = 120;
	fse->min_height = 120;
	fse->max_width = 4096;
	fse->max_width = 4096;

	return 0;
}

static int mscaler_get_fmt(struct v4l2_subdev *sd,
		       struct v4l2_subdev_pad_config *cfg,
		       struct v4l2_subdev_format *format)
{
	struct mscaler_device *mscaler = v4l2_get_subdevdata(sd);
	struct v4l2_mbus_framefmt *framefmt = NULL;
	//printk("------%s, %d, sd->name: %s\n", __func__, __LINE__, sd->name);

	if(format->which == V4L2_SUBDEV_FORMAT_TRY) {
		framefmt = v4l2_subdev_get_try_format(sd, cfg, format->pad);
	} else {
		framefmt = &mscaler->formats[format->pad].format;
	}

	format->format = *framefmt;

	return 0;
}

static int mscaler_set_fmt(struct v4l2_subdev *sd,
		       struct v4l2_subdev_pad_config *cfg,
		       struct v4l2_subdev_format *format)
{
	struct mscaler_device *mscaler = v4l2_get_subdevdata(sd);
	struct media_pad *remote = NULL;
	struct v4l2_subdev *remote_sd = NULL;
	struct v4l2_subdev_format remote_subdev_fmt;
	int ret = 0;

	printk("%s, %d, width: %d, height: %d, mbus_code: %x\n", __func__, __LINE__, format->format.width, format->format.height, format->format.code);

	mscaler->formats[format->pad] = *format;

	remote = media_entity_remote_pad(&mscaler->pads[MSCALER_PAD_SINK]);
	remote_sd = media_entity_to_v4l2_subdev(remote->entity);

	remote_subdev_fmt.pad = remote->index;
	remote_subdev_fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	ret = v4l2_subdev_call(remote_sd, pad, get_fmt, NULL, &remote_subdev_fmt);
	if(ret < 0) {
		dev_err(mscaler->dev, "Failed to get_fmt from remote pad\n");
		return -EINVAL;
	}

	memcpy(&mscaler->formats[MSCALER_PAD_SINK], &remote_subdev_fmt, sizeof(struct v4l2_subdev_format));

	/*TODO:fix crop info.*/

	mscaler->crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	mscaler->crop.c.left = 0;
	mscaler->crop.c.top = 0;
	mscaler->crop.c.width = format->format.width;
	mscaler->crop.c.height = format->format.height;

	return 0;
}

static const struct v4l2_subdev_pad_ops mscaler_subdev_pad_ops = {
        .enum_mbus_code         = mscaler_enum_mbus_code,
        .enum_frame_size        = mscaler_enum_frame_size,
        .get_fmt                = mscaler_get_fmt,
        .set_fmt                = mscaler_set_fmt,
};


static unsigned int ch_to_pad[] = {
	MSCALER_PAD_SOURCE_CH0,
	MSCALER_PAD_SOURCE_CH1,
	MSCALER_PAD_SOURCE_CH2
};

static int mscaler_stream_enable(struct mscaler_device *mscaler, int ch)
{
	struct v4l2_subdev_format *input_fmt = &mscaler->formats[MSCALER_PAD_SINK];
	struct v4l2_subdev_format *output_fmt = &mscaler->formats[ch_to_pad[ch]];
	unsigned long flags = 0;
	unsigned int step_w = 0, step_h = 0;
	unsigned int val = 0;
	int ret = 0;
	struct isp_video_buffer *isp_buffer = NULL;
	struct isp_video_buffer *tmp = NULL;
	dma_addr_t y_addr = 0;
	dma_addr_t uv_addr = 0;

	printk("-----input_fmt->format.width: %d, input_fmt->format.height: %d\n", input_fmt->format.width, input_fmt->format.height);
	printk("-----output_fmt->format.width: %d, output_fmt->format.height: %d\n", output_fmt->format.width, output_fmt->format.height);

	spin_lock_irqsave(&mscaler->lock, flags);
	/*1. mscaler_hw_configure.*/
	mscaler_reg_writel(mscaler, CHx_RSZ_OSIZE(ch), (output_fmt->format.width << 16) | (output_fmt->format.height));

	step_w = input_fmt->format.width * 512 / output_fmt->format.width;
	step_h = input_fmt->format.height * 512 / output_fmt->format.height;

	mscaler_reg_writel(mscaler, CHx_RSZ_STEP(ch), step_w << 16 | step_h);

	/*Crop info.*/
	mscaler_reg_writel(mscaler, CHx_CROP_OPOS(ch), (mscaler->crop.c.left << 16) | mscaler->crop.c.top);
	mscaler_reg_writel(mscaler, CHx_CROP_OSIZE(ch), (mscaler->crop.c.width << 16) | mscaler->crop.c.height);

	mscaler_reg_writel(mscaler, CHx_DMAOUT_Y_STRI(ch), mscaler->crop.c.width);
	mscaler_reg_writel(mscaler, CHx_DMAOUT_UV_STRI(ch), mscaler->crop.c.width);

	val = mscaler_reg_readl(mscaler, MSCA_DMAOUT_ARB);
	val |= 1 << (ch + 1);
	mscaler_reg_writel(mscaler, MSCA_DMAOUT_ARB, val);

	/*2. mscaler_hw_start*/
	val = mscaler_reg_readl(mscaler, MSCA_CH_EN);
	val |= 1 << ch;
	val |= (1 << 8) << ch; //fix_vrsz_en
	val |= (1 << 11) << ch; //fix_hrsz_en
	mscaler_reg_writel(mscaler, MSCA_CH_EN, val);

	mscaler->state[ch] = 1;

	/*因为ISP复位的执行时间时在stream_on的时候，这个时候上层qbuf已经调用了，
	 * 如果部将这些buffer重新加入硬件队列，则会出现问题.*/
	list_for_each_entry_safe(isp_buffer, tmp, &mscaler->dma_queued_list[ch], list_entry) {
		struct vb2_buffer *vb2_buf = &isp_buffer->vb2.vb2_buf;
		/*Y*/
		y_addr = ingenic_vb2_dma_contig_plane_dma_addr(vb2_buf, 0);
		mscaler_reg_writel(mscaler, CHx_DMAOUT_Y_ADDR(ch), y_addr);

		uv_addr = y_addr + isp_buffer->uv_offset;
		if(vb2_buf->num_planes == 2) {
			uv_addr = ingenic_vb2_dma_contig_plane_dma_addr(vb2_buf, 1);
		}
		mscaler_reg_writel(mscaler, CHx_DMAOUT_UV_ADDR(ch), uv_addr);
	}

	spin_unlock_irqrestore(&mscaler->lock, flags);

	return ret;
}

static int mscaler_stream_disable(struct mscaler_device *mscaler, int ch)
{
	unsigned long flags = 0;
	unsigned int val = 0;
	int ret = 0;
	unsigned int timeout = 0xffffff;
	struct isp_video_buffer *isp_buffer = NULL;
	struct isp_video_buffer *tmp = NULL;


	spin_lock_irqsave(&mscaler->lock, flags);

	val = mscaler_reg_readl(mscaler, MSCA_CH_EN);
	val &= ~(1 << ch);
	mscaler_reg_writel(mscaler, MSCA_CH_EN, val);

	spin_unlock_irqrestore(&mscaler->lock, flags);

	/* polling status to make sure channel stopped.*/
	do {
		val = mscaler_reg_readl(mscaler, MSCA_CH_STA);

	} while(!!(val & 0x7 << 0) && --timeout);


	if(!timeout) {
		dev_err(mscaler->dev, "[Warning] mscaler disable timeout!\n");
	}

	spin_lock_irqsave(&mscaler->lock, flags);

	/* clear fifo*/
	mscaler_reg_writel(mscaler, CHx_DMAOUT_Y_ADDR_CLR(ch), 1);
	mscaler_reg_writel(mscaler, CHx_DMAOUT_UV_ADDR_CLR(ch), 1);
	mscaler_reg_writel(mscaler, CHx_DMAOUT_Y_LAST_ADDR_CLR(ch), 1);
	mscaler_reg_writel(mscaler, CHx_DMAOUT_UV_LAST_ADDR_CLR(ch), 1);


	list_for_each_entry_safe(isp_buffer, tmp, &mscaler->dma_queued_list[ch], list_entry) {
		struct vb2_buffer *vb2_buf = &isp_buffer->vb2.vb2_buf;
		vb2_buffer_done(vb2_buf, VB2_BUF_STATE_ERROR);
		list_del(&isp_buffer->list_entry);
	}

	mscaler->state[ch] = 0;
	spin_unlock_irqrestore(&mscaler->lock, flags);

	return ret;
}

static int mscaler_subdev_s_stream(struct v4l2_subdev *sd, int enable)
{
	struct mscaler_device *mscaler = v4l2_get_subdevdata(sd);
	struct isp_video_device *ispvideo = v4l2_get_subdev_hostdata(sd);
	int ch = 0;
	int i = 0, ret = 0;

	for(i = 0; i < MSCALER_MAX_CH; i++) {
		if(ispvideo == &mscaler->ispvideo[i]) {
			ch = i;
			break;
		}
	}

	//printk("%s,%d, sd->video_device->name:%s, ispvideo: %p, ch = %d, enable = %d\n", __func__, __LINE__, sd->devnode->name, ispvideo, ch, enable);
	/*获取当前channel.*/
	if(enable) {
		ret = mscaler_stream_enable(mscaler, ch);
		//dump_mscaler_regs(mscaler);
	} else {
		ret = mscaler_stream_disable(mscaler, ch);
	}

	if(ret < 0) {
		dev_err(mscaler->dev, "enable stream error!\n");
		return -EINVAL;
	}

	return 0;
}


static const struct v4l2_subdev_video_ops mscaler_subdev_video_ops = {
        .s_stream = mscaler_subdev_s_stream,
};


static const struct v4l2_subdev_ops mscaler_subdev_ops = {
        .core = &mscaler_subdev_core_ops,
        .pad = &mscaler_subdev_pad_ops,
        .video = &mscaler_subdev_video_ops,
};


static int mscaler_video_qbuf(struct isp_video_device *ispvideo, struct isp_video_buffer *isp_buffer)
{
	struct mscaler_device *mscaler = ispvideo->ispcam->mscaler;
	struct vb2_buffer *vb2_buf = NULL;
	int ch = 0;
	unsigned long flags = 0;
	unsigned int fifo_empty = 0;
	unsigned int fifo_full = 0;
	unsigned int y_fifo_st = 0, uv_fifo_st = 0;
	dma_addr_t y_addr = 0;
	dma_addr_t uv_addr = 0;

	int i = 0;
	for(i = 0; i < MSCALER_MAX_CH; i++) {
		if(ispvideo == &mscaler->ispvideo[i]) {
			ch = i;
			break;
		}
	}

	spin_lock_irqsave(&mscaler->lock, flags);
	y_fifo_st = mscaler_reg_readl(mscaler, CHx_Y_ADDR_FIFO_STA(ch));
	uv_fifo_st = mscaler_reg_readl(mscaler, CHx_UV_ADDR_FIFO_STA(ch));

	if((y_fifo_st & (1<<0)) && (uv_fifo_st & (1<<0))) {
		fifo_empty = 1;
	} else if((y_fifo_st & (1<<1)) || (uv_fifo_st & (1<<1))) {
		fifo_full = 1;
	}


	if(!fifo_full) {

		vb2_buf = &isp_buffer->vb2.vb2_buf;

		list_add_tail(&isp_buffer->list_entry, &mscaler->dma_queued_list[ch]);

		/* 如果stream on了，就写入硬件.*/
		if(mscaler->state[ch] == 1) {
			/*Y*/
			y_addr = ingenic_vb2_dma_contig_plane_dma_addr(vb2_buf, 0);
			mscaler_reg_writel(mscaler, CHx_DMAOUT_Y_ADDR(ch), y_addr);

			uv_addr = y_addr + isp_buffer->uv_offset;
			/*UV*/
			if(vb2_buf->num_planes == 2) {
				uv_addr = ingenic_vb2_dma_contig_plane_dma_addr(vb2_buf, 1);
			}
			mscaler_reg_writel(mscaler, CHx_DMAOUT_UV_ADDR(ch), uv_addr);
		}
	}

	spin_unlock_irqrestore(&mscaler->lock, flags);

	//printk("-----%s,%d queued buffer y_addr: 0x%08x, uv: 0x%08x on ch: %d\n", __func__, __LINE__, y_addr, uv_addr, ch);

	return 0;
}

static const struct isp_video_ops mscaler_video_ops = {
	.find_format 	= mscaler_find_format,
	.qbuf		= mscaler_video_qbuf,
};

static ssize_t
dump_mscaler(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct mscaler_device *mscaler = dev_get_drvdata(dev);
	char *p = buf;
	int i = 0;

	p += sprintf(p, "MSCA_CTRL	:0x%08x\n", mscaler_reg_readl(mscaler, MSCA_CTRL));
	p += sprintf(p, "MSCA_CH_EN     :0x%08x\n", mscaler_reg_readl(mscaler, MSCA_CH_EN));
	p += sprintf(p, "MSCA_CH_STA    :0x%08x\n", mscaler_reg_readl(mscaler, MSCA_CH_STA));
	p += sprintf(p, "MSCA_INT_STA   :0x%08x\n", mscaler_reg_readl(mscaler, MSCA_INT_STA));
	p += sprintf(p, "MSCA_INT_MSK   :0x%08x\n", mscaler_reg_readl(mscaler, MSCA_INT_MSK));
	p += sprintf(p, "MSCA_INT_CLR   :0x%08x\n", mscaler_reg_readl(mscaler, MSCA_INT_CLR));
	p += sprintf(p, "MSCA_DMAOUT_ARB:0x%08x\n", mscaler_reg_readl(mscaler, MSCA_DMAOUT_ARB));
	p += sprintf(p, "MSCA_CLK_GATE_EN:0x%08x\n", mscaler_reg_readl(mscaler, MSCA_CLK_GATE_EN));
	p += sprintf(p, "MSCA_CLK_DIS   :0x%08x\n", mscaler_reg_readl(mscaler, MSCA_CLK_DIS));
	p += sprintf(p, "MSCA_SRC_IN    :0x%08x\n", mscaler_reg_readl(mscaler, MSCA_SRC_IN));
	p += sprintf(p, "MSCA_SRC_SIZE  :0x%08x\n", mscaler_reg_readl(mscaler, MSCA_SRC_SIZE));
	p += sprintf(p, "MSCA_GLO_RSZ_COEF_WR:0x%08x\n", mscaler_reg_readl(mscaler, MSCA_GLO_RSZ_COEF_WR));
	p += sprintf(p, "MSCA_SYS_PRO_CLK_EN :0x%08x\n", mscaler_reg_readl(mscaler, MSCA_SYS_PRO_CLK_EN));
	p += sprintf(p, "MSCA_DS0_CLK_NUM    :0x%08x\n", mscaler_reg_readl(mscaler, MSCA_DS0_CLK_NUM));
	p += sprintf(p, "MSCA_DS1_CLK_NUM    :0x%08x\n", mscaler_reg_readl(mscaler, MSCA_DS1_CLK_NUM));
	p += sprintf(p, "MSCA_DS2_CLK_NUM    :0x%08x\n", mscaler_reg_readl(mscaler, MSCA_DS2_CLK_NUM));

	for(i = 0; i < MSCALER_MAX_CH; i++) {

		p += sprintf(p, "CHx_RSZ_OSIZE(%d)	:0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_RSZ_OSIZE(i)));
		p += sprintf(p, "CHx_RSZ_STEP(%d)       :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_RSZ_STEP(i)));
		p += sprintf(p, "CHx_CROP_OPOS(%d)      :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_CROP_OPOS(i)));
		p += sprintf(p, "CHx_CROP_OSIZE(%d)     :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_CROP_OSIZE(i)));
		p += sprintf(p, "CHx_FRA_CTRL_LOOP(%d)  :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_FRA_CTRL_LOOP(i)));
		p += sprintf(p, "CHx_FRA_CTRL_MASK(%d)  :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_FRA_CTRL_MASK(i)));
		p += sprintf(p, "CHx_MS0_POS(%d)        :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_MS0_POS(i)));
		p += sprintf(p, "CHx_MS0_SIZE(%d)       :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_MS0_SIZE(i)));
		p += sprintf(p, "CHx_MS0_VALUE(%d)      :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_MS0_VALUE(i)));
		p += sprintf(p, "CHx_MS1_POS(%d)        :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_MS1_POS(i)));
		p += sprintf(p, "CHx_MS1_SIZE(%d)       :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_MS1_SIZE(i)));
		p += sprintf(p, "CHx_MS1_VALUE(%d)      :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_MS1_VALUE(i)));
		p += sprintf(p, "CHx_MS2_POS(%d)        :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_MS2_POS(i)));
		p += sprintf(p, "CHx_MS2_SIZE(%d)       :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_MS2_SIZE(i)));
		p += sprintf(p, "CHx_MS2_VALUE(%d)      :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_MS2_VALUE(i)));
		p += sprintf(p, "CHx_MS3_POS(%d)        :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_MS3_POS(i)));
		p += sprintf(p, "CHx_MS3_SIZE(%d)       :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_MS3_SIZE(i)));
		p += sprintf(p, "CHx_MS3_VALUE(%d)      :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_MS3_VALUE(i)));
		p += sprintf(p, "CHx_OUT_FMT(%d)        :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_OUT_FMT(i)));
		p += sprintf(p, "CHx_DMAOUT_Y_ADDR(%d)  :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_DMAOUT_Y_ADDR(i)));
		p += sprintf(p, "CHx_Y_ADDR_FIFO_STA(%d):0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_Y_ADDR_FIFO_STA(i)));
	//	p += sprintf(p, "CHx_DMAOUT_Y_LAST_ADDR(%d)              :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_DMAOUT_Y_LAST_ADDR(i)));
	//	p += sprintf(p, "CHx_DMAOUT_Y_LAST_STATS_NUM(%d)         :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_DMAOUT_Y_LAST_STATS_NUM(i)));
		p += sprintf(p, "CHx_Y_LAST_ADDR_FIFO_STA(%d):0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_Y_LAST_ADDR_FIFO_STA(i)));
		p += sprintf(p, "CHx_DMAOUT_Y_STRI(%d)       :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_DMAOUT_Y_STRI(i)));
		p += sprintf(p, "CHx_DMAOUT_UV_ADDR(%d)      :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_DMAOUT_UV_ADDR(i)));
		p += sprintf(p, "CHx_UV_ADDR_FIFO_STA(%d)    :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_UV_ADDR_FIFO_STA(i)));
	//	p += sprintf(p, "CHx_DMAOUT_UV_LAST_ADDR(%d) :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_DMAOUT_UV_LAST_ADDR(i)));
	//	p += sprintf(p, "CHx_DMAOUT_UV_LAST_STATS_NUM(%d)        :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_DMAOUT_UV_LAST_STATS_NUM(i)));
		p += sprintf(p, "CHx_UV_LAST_ADDR_FIFO_STA(%d):0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_UV_LAST_ADDR_FIFO_STA(i)));
		p += sprintf(p, "CHx_DMAOUT_UV_STRI(%d)       :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_DMAOUT_UV_STRI(i)));
		p += sprintf(p, "CHx_DMAOUT_Y_ADDR_CLR(%d)    :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_DMAOUT_Y_ADDR_CLR(i)));
		p += sprintf(p, "CHx_DMAOUT_UV_ADDR_CLR(%d)   :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_DMAOUT_UV_ADDR_CLR(i)));
		p += sprintf(p, "CHx_DMAOUT_Y_LAST_ADDR_CLR(%d):0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_DMAOUT_Y_LAST_ADDR_CLR(i)));
		p += sprintf(p, "CHx_DMAOUT_UV_LAST_ADDR_CLR(%d):0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_DMAOUT_UV_LAST_ADDR_CLR(i)));
		p += sprintf(p, "CHx_DMAOUT_Y_ADDR_SEL(%d)    :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_DMAOUT_Y_ADDR_SEL(i)));
		p += sprintf(p, "CHx_DMAOUT_UV_ADDR_SEL(%d)   :0x%08x\n", i, mscaler_reg_readl(mscaler, CHx_DMAOUT_UV_ADDR_SEL(i)));
	}

	return p - buf;
}


static DEVICE_ATTR(dump_mscaler, S_IRUGO|S_IWUSR, dump_mscaler, NULL);

static struct attribute *mscaler_debug_attrs[] = {
        &dev_attr_dump_mscaler.attr,
	NULL,
};

static struct attribute_group mscaler_debug_attr_group = {
        .name   = "debug",
        .attrs  = mscaler_debug_attrs,
};


static void __maybe_unused dump_mscaler_regs(struct mscaler_device *mscaler)
{
	char *buf = kzalloc(4 * 1024, GFP_KERNEL);
	int ret = 0;

	ret = dump_mscaler(mscaler->dev, NULL, buf);

	printk("%s", buf);

	kfree(buf);
}

static int mscaler_comp_bind(struct device *comp, struct device *master,
                              void *master_data)
{
	struct mscaler_device *mscaler = dev_get_drvdata(comp);
	struct ispcam_device *ispcam = (struct ispcam_device *)master_data;
	struct v4l2_device *v4l2_dev = &ispcam->v4l2_dev;
	struct v4l2_subdev *sd = &mscaler->sd;
	int i = 0;
	int ret = 0;

	//dev_info(comp, "----dev_name(comp): %s----%s, %d \n", dev_name(comp), __func__, __LINE__);

	/* link subdev to master.*/
	mscaler->ispcam = (void *)ispcam;
	ispcam->mscaler = mscaler;

	/*1. register supported subdev ctrls.*/


	/*2. init v4l2_subdev*/

	v4l2_subdev_init(sd, &mscaler_subdev_ops);

	sd->owner = THIS_MODULE;
	sd->flags = V4L2_SUBDEV_FL_HAS_DEVNODE;
	strscpy(sd->name, dev_name(comp), sizeof(sd->name));
	v4l2_set_subdevdata(sd, mscaler);


	/* init mscaler pads. */
	mscaler->pads = kzalloc(sizeof(struct media_pad) * MSCALER_NUM_PADS, GFP_KERNEL);
	if(!mscaler->pads) {
		ret = -ENOMEM;
		goto err_alloc_pads;
	}
	mscaler->pads[0].index = MSCALER_PAD_SINK;
	mscaler->pads[0].flags = MEDIA_PAD_FL_SINK;
	mscaler->pads[1].index = MSCALER_PAD_SOURCE_CH0;
	mscaler->pads[1].flags = MEDIA_PAD_FL_SOURCE;
	mscaler->pads[2].index = MSCALER_PAD_SOURCE_CH1;
	mscaler->pads[2].flags = MEDIA_PAD_FL_SOURCE;
	mscaler->pads[3].index = MSCALER_PAD_SOURCE_CH2;
	mscaler->pads[3].flags = MEDIA_PAD_FL_SOURCE;

	ret = media_entity_init(&sd->entity, MSCALER_NUM_PADS, mscaler->pads, 0);

	/*3. register v4l2_subdev*/
	ret = v4l2_device_register_subdev(v4l2_dev, sd);
	if(ret < 0) {
		dev_err(comp, "Failed to register v4l2_subdev for mscaler\n");
		goto err_subdev_register;
	}


	for(i = 0; i < MSCALER_MAX_CH; i++) {
		struct isp_video_device *ispvideo = &mscaler->ispvideo[i];
		char name[32];
		sprintf(name, "%s-ch%d", dev_name(mscaler->dev), i);

		ispvideo->ispcam = ispcam;
		ret = isp_video_init(ispvideo, name, &mscaler_video_ops);
		if(ret < 0){
			/*TODO*/
		}

		ret = isp_video_register(ispvideo, &ispcam->v4l2_dev);
		if(ret < 0){
			/*TODO*/
		}
	}


	return 0;
err_subdev_register:
err_alloc_pads:
	return ret;
}


static void mscaler_comp_unbind(struct device *comp, struct device *master,
                                 void *master_data)
{
        struct mscaler_device *mscaler = dev_get_drvdata(comp);

	dev_info(comp, "----TODO: %p----%s, %d \n", mscaler, __func__, __LINE__);

}

static const struct component_ops mscaler_comp_ops = {
        .bind = mscaler_comp_bind,
        .unbind = mscaler_comp_unbind,
};




static int ingenic_mscaler_probe(struct platform_device *pdev)
{

	struct mscaler_device *mscaler = NULL;
	struct resource *regs = NULL;
	int ret = 0;
	int i = 0;

	mscaler = kzalloc(sizeof(struct mscaler_device), GFP_KERNEL);
	if(!mscaler) {
		pr_err("Failed to alloc mscaler dev [%s]\n", pdev->name);
		return -ENOMEM;
	}

	mscaler->dev = &pdev->dev;
	platform_set_drvdata(pdev, mscaler);


	mscaler->irq = platform_get_irq(pdev, 0);
	if(mscaler->irq < 0) {
		dev_warn(&pdev->dev, "No MScaler IRQ specified\n");
	}

	regs = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(!regs) {
		dev_err(&pdev->dev, "No iomem resource!\n");
		goto err_get_resource;
	}

	mscaler->iobase = devm_ioremap_resource(&pdev->dev, regs);
	if(!mscaler->iobase) {
		goto err_ioremap;
	}

	spin_lock_init(&mscaler->lock);

	for(i = 0; i < MSCALER_MAX_CH; i++) {
		INIT_LIST_HEAD(&mscaler->dma_queued_list[i]);
		INIT_LIST_HEAD(&mscaler->dma_pending_list[i]);
	}

	ret = sysfs_create_group(&mscaler->dev->kobj, &mscaler_debug_attr_group);
	if (ret) {
		dev_err(mscaler->dev, "device create sysfs group failed\n");

		ret = -EINVAL;
		goto err_sys_group;
	}


	ret = component_add(mscaler->dev, &mscaler_comp_ops);
	if(ret < 0) {
		dev_err(mscaler->dev, "Failed to add component mscaler!\n");
	}

	return 0;
err_sys_group:
err_ioremap:
err_get_resource:

	return ret;
}



static int ingenic_mscaler_remove(struct platform_device *pdev)
{

	return 0;
}



static const struct of_device_id ingenic_mscaler_dt_match[] = {
        { .compatible = "ingenic,x2000-mscaler" },
        { }
};

MODULE_DEVICE_TABLE(of, ingenic_mscaler_dt_match);

static int __maybe_unused ingenic_mscaler_runtime_suspend(struct device *dev)
{
        return 0;
}

static int __maybe_unused ingenic_mscaler_runtime_resume(struct device *dev)
{
        return 0;
}

static const struct dev_pm_ops ingenic_mscaler_pm_ops = {
        SET_SYSTEM_SLEEP_PM_OPS(pm_runtime_force_suspend,
                                pm_runtime_force_resume)
        SET_RUNTIME_PM_OPS(ingenic_mscaler_runtime_suspend, ingenic_mscaler_runtime_resume, NULL)
};

static struct platform_driver ingenic_mscaler_driver = {
        .probe = ingenic_mscaler_probe,
        .remove = ingenic_mscaler_remove,
        .driver = {
                .name = "ingenic-mscaler",
                .of_match_table = ingenic_mscaler_dt_match,
                .pm = &ingenic_mscaler_pm_ops,
        },
};

module_platform_driver(ingenic_mscaler_driver);

MODULE_ALIAS("platform:ingenic-mscaler");
MODULE_DESCRIPTION("ingenic mscaler subsystem");
MODULE_AUTHOR("qipengzhen <aric.pzqi@ingenic.com>");
MODULE_LICENSE("GPL v2");

