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
#include <linux/clk.h>

#include <media/media-device.h>
#include <media/v4l2-async.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-event.h>


#include "isp-drv.h"
#include "vic-regs.h"


struct isp_video_format vic_mipi_formats[] = {
	/* RAW8 */
	{
		.name     = "RAW8, 8  BGBG.. GRGR..",
		.fourcc   = V4L2_PIX_FMT_SBGGR8,
		.depth    = {8},
		.num_planes = 1,
		.mbus_code = MEDIA_BUS_FMT_SBGGR8_1X8,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
	{
		.name     = "RAW8, 8  GBGB.. RGRG..",
		.fourcc   = V4L2_PIX_FMT_SGBRG8,
		.depth    = {8},
		.num_planes = 1,
		.mbus_code = MEDIA_BUS_FMT_SGBRG8_1X8,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
	{
		.name     = "RAW8, 8  GRGR.. BGBG..",
		.fourcc   = V4L2_PIX_FMT_SGRBG8,
		.depth    = {8},
		.num_planes = 1,
		.mbus_code = MEDIA_BUS_FMT_SGRBG8_1X8,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
	{
		.name     = "RAW8, 8  RGRG.. GBGB..",
		.fourcc   = V4L2_PIX_FMT_SRGGB8,
		.depth    = {8},
		.num_planes = 1,
		.mbus_code = MEDIA_BUS_FMT_SRGGB8_1X8,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
	/* RAW10 */
	{
		.name     = "RAW10, 10  BGBG.. GRGR..",
		.fourcc   = V4L2_PIX_FMT_SBGGR10,
		.depth    = {10},
		.num_planes = 1,
		.mbus_code = MEDIA_BUS_FMT_SBGGR10_1X10,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
	{
		.name     = "RAW10, 10  GBGB.. RGRG..",
		.fourcc   = V4L2_PIX_FMT_SGBRG10,
		.depth    = {10},
		.num_planes = 1,
		.mbus_code = MEDIA_BUS_FMT_SGBRG10_1X10,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
	{
		.name     = "RAW10, 10  GRGR.. BGBG..",
		.fourcc   = V4L2_PIX_FMT_SGRBG10,
		.depth    = {10},
		.num_planes = 1,
		.mbus_code = MEDIA_BUS_FMT_SGRBG10_1X10,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
	{
		.name     = "RAW10, 10  RGRG.. GBGB..",
		.fourcc   = V4L2_PIX_FMT_SRGGB10,
		.depth    = {10},
		.num_planes = 1,
		.mbus_code = MEDIA_BUS_FMT_SRGGB10_1X10,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
	/* RAW12 */
	{
		.name     = "RAW12, 12  BGBG.. GRGR..",
		.fourcc   = V4L2_PIX_FMT_SBGGR12,
		.depth    = {12},
		.num_planes = 1,
		.mbus_code = MEDIA_BUS_FMT_SBGGR12_1X12,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
	{
		.name     = "RAW12, 12  GBGB.. RGRG..",
		.fourcc   = V4L2_PIX_FMT_SGBRG12,
		.depth    = {12},
		.num_planes = 1,
		.mbus_code = MEDIA_BUS_FMT_SGBRG12_1X12,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
	{
		.name     = "RAW12, 12  GRGR.. BGBG..",
		.fourcc   = V4L2_PIX_FMT_SGRBG12,
		.depth    = {12},
		.num_planes = 1,
		.mbus_code = MEDIA_BUS_FMT_SGRBG12_1X12,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
	{
		.name     = "RAW12, 12  RGRG.. GBGB..",
		.fourcc   = V4L2_PIX_FMT_SRGGB12,
		.depth    = {12},
		.num_planes = 1,
		.mbus_code = MEDIA_BUS_FMT_SRGGB12_1X12,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
};

struct isp_video_format vic_dvp_formats = {

};


static inline void vic_reg_writel(struct vic_device *vic, unsigned int reg, unsigned int val)
{
	writel(val, vic->iobase + reg);
}

static inline unsigned int vic_reg_readl(struct vic_device *vic, unsigned int reg)
{
	return readl(vic->iobase + reg);
}


static int ingenic_vic_parse_dt(struct vic_device * vic)
{
	struct device *dev = vic->dev;
	int ret = 0;

	return ret;
}

static const struct v4l2_subdev_core_ops vic_subdev_core_ops = {
        .log_status = v4l2_ctrl_subdev_log_status,
        .subscribe_event = v4l2_ctrl_subdev_subscribe_event,
        .unsubscribe_event = v4l2_event_subdev_unsubscribe,
};

static int vic_subdev_get_fmt(struct v4l2_subdev *sd,
		       struct v4l2_subdev_pad_config *cfg,
		       struct v4l2_subdev_format *format)
{
	struct vic_device *vic = v4l2_get_subdevdata(sd);
	struct isp_async_device *isd = &vic->ispcam->isd[0];	/*TODO: 默认支持一个asd.*/
	struct media_pad *remote = NULL;
	struct v4l2_subdev *remote_sd = NULL;
	struct v4l2_subdev_format remote_subdev_fmt;
	int ret = 0;

	remote = media_entity_remote_pad(&vic->pads[VIC_PAD_SINK]);
	remote_sd = media_entity_to_v4l2_subdev(remote->entity);

	/*获取源当前格式，复制到输出格式.*/
	remote_subdev_fmt.pad = remote->index;
	remote_subdev_fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	ret = v4l2_subdev_call(remote_sd, pad, get_fmt, NULL, &remote_subdev_fmt);
	if(ret < 0) {
		dev_err(vic->dev, "Failed to get_fmt from remote pad\n");
		return -EINVAL;
	}

	memcpy(&format->format, &remote_subdev_fmt.format, sizeof(format->format));

	vic->formats[VIC_PAD_SINK] = vic->formats[VIC_PAD_SOURCE] = *format;

	//printk("----%s, %d, format->pad: %d\n", __func__, __LINE__, format->pad);
	return 0;
}

static const struct v4l2_subdev_pad_ops vic_subdev_pad_ops = {
	.get_fmt		= vic_subdev_get_fmt,
};


static int vic_cfg_stream_mipi(struct vic_device *vic)
{
	struct v4l2_subdev_format *input_fmt = &vic->formats[VIC_PAD_SINK];
	struct v4l2_of_bus_mipi_csi2 *mipi_csi2 = ispcam_get_bus_mipi_csi2(vic->ispcam);
	//unsigned int timeout = 10000;

	printk("%s, %d, input_fmt->format.width: %d, intput_fmt->format.height: %d\n",
		__func__, __LINE__, input_fmt->format.width, input_fmt->format.height);

	vic_reg_writel(vic, VIC_INTF_TYPE, INTF_TYPE_MIPI);

	switch(input_fmt->format.code) {
		case MEDIA_BUS_FMT_SBGGR8_1X8:
		case MEDIA_BUS_FMT_SGBRG8_1X8:
		case MEDIA_BUS_FMT_SGRBG8_1X8:
		case MEDIA_BUS_FMT_SRGGB8_1X8:
			vic_reg_writel(vic, VIC_IN_CSI_FMT,MIPI_RAW8);//RAW8
			break;
		case    MEDIA_BUS_FMT_SBGGR10_1X10:
		case    MEDIA_BUS_FMT_SGBRG10_1X10:
		case    MEDIA_BUS_FMT_SGRBG10_1X10:
		case    MEDIA_BUS_FMT_SRGGB10_1X10:
			vic_reg_writel(vic, VIC_IN_CSI_FMT,MIPI_RAW10);//RAW10
			break;
		case    MEDIA_BUS_FMT_SBGGR12_1X12:
		case    MEDIA_BUS_FMT_SGBRG12_1X12:
		case    MEDIA_BUS_FMT_SGRBG12_1X12:
		case    MEDIA_BUS_FMT_SRGGB12_1X12:
			vic_reg_writel(vic, VIC_IN_CSI_FMT,MIPI_RAW12);//RAW12
			break;
		default:
			dev_err(vic->dev, "unsupported mbus fmt: %x\n", input_fmt->format.code);
			return -EINVAL;
	}

	vic_reg_writel(vic, MIPI_HCROP_CH0, (input_fmt->format.width << 16) | 0);

	return 0;
}

static int vic_cfg_stream_dvp(struct vic_device *vic)
{
	struct v4l2_subdev_format *input_fmt = &vic->formats[VIC_PAD_SINK];
	struct v4l2_of_bus_parallel *parallel = ispcam_get_bus_parallel(vic->ispcam);
	unsigned int input_cfg = 0;

	printk("parallel->flags: 0x%x\n", parallel->flags);
	printk("parallel->bus_width: %d\n", parallel->bus_width);
	printk("parallel->data_shift: %d\n", parallel->data_shift);
	vic_reg_writel(vic, VIC_INTF_TYPE, INTF_TYPE_DVP);
	switch(input_fmt->format.code) {
		case MEDIA_BUS_FMT_SBGGR8_1X8:
		case MEDIA_BUS_FMT_SGBRG8_1X8:
		case MEDIA_BUS_FMT_SGRBG8_1X8:
		case MEDIA_BUS_FMT_SRGGB8_1X8:
			if(parallel->data_shift) {
				input_cfg = DVP_RAW8 | DVP_RAW_ALIG;
			} else {
				input_cfg = DVP_RAW8;
			}
			break;
		case MEDIA_BUS_FMT_SBGGR10_1X10:
		case MEDIA_BUS_FMT_SGBRG10_1X10:
		case MEDIA_BUS_FMT_SGRBG10_1X10:
		case MEDIA_BUS_FMT_SRGGB10_1X10:
			if(parallel->data_shift) {
				input_cfg = DVP_RAW10 | DVP_RAW_ALIG;
			} else {
				input_cfg = DVP_RAW10;
			}
			break;
		case MEDIA_BUS_FMT_SBGGR12_1X12:
		case MEDIA_BUS_FMT_SGBRG12_1X12:
		case MEDIA_BUS_FMT_SGRBG12_1X12:
		case MEDIA_BUS_FMT_SRGGB12_1X12:
			input_cfg = DVP_RAW12;
			break;
		case MEDIA_BUS_FMT_UYVY8_1_5X8 :
		case MEDIA_BUS_FMT_VYUY8_1_5X8 :
		case MEDIA_BUS_FMT_YUYV8_1_5X8 :
		case MEDIA_BUS_FMT_YVYU8_1_5X8 :
		case MEDIA_BUS_FMT_YUYV8_1X16 :
			input_cfg = DVP_YUV422_8BIT;
			break;

		default:
			dev_err(vic->dev, "unsupported input formats\n");
			return -EINVAL;
	}
	if(parallel->flags & V4L2_MBUS_HSYNC_ACTIVE_LOW) {
		input_cfg |= HSYN_POLAR;
	}
	if(parallel->flags & V4L2_MBUS_VSYNC_ACTIVE_LOW) {
		input_cfg |= VSYN_POLAR;
	}
	vic_reg_writel(vic, VIC_IN_DVP, input_cfg);
	/*TODO: hblank and vblank*/

	vic_reg_writel(vic, VIC_IN_HOR_PARA0, input_fmt->format.width + 0 /*hblank*/);

	return 0;
}

static int vic_subdev_s_stream(struct v4l2_subdev *sd, int enable)
{
	struct vic_device *vic = v4l2_get_subdevdata(sd);
	struct isp_async_device *isd = &vic->ispcam->isd[0];	/*TODO: 默认支持一个asd.*/
	struct v4l2_subdev_format *input_fmt = &vic->formats[VIC_PAD_SINK];
	int ret = 0;

	/*hw configure.*/
	if(enable) {
		/* enable timestamp .*/
		vic_reg_writel(vic, VIC_TS_COUNTER, 1);
		vic_reg_writel(vic, VIC_TS_DMA_OFFSET, 0);
		vic_reg_writel(vic, VIC_TS_MS_CH0_OFFSET, 0);
		vic_reg_writel(vic, VIC_TS_MS_CH1_OFFSET, 0);
		vic_reg_writel(vic, VIC_TS_MS_CH2_OFFSET, 0);
		vic_reg_writel(vic, VIC_TS_ENABLE, TS_COUNTER_EN |
				TS_VIC_DMA_EN | TS_MS_CH0_EN |
				TS_MS_CH1_EN | TS_MS_CH2_EN);

		/*confgure hw and start*/
		if(isd->bus_type == V4L2_MBUS_CSI2) {
			ret = vic_cfg_stream_mipi(vic);
		} else {
			ret = vic_cfg_stream_dvp(vic);
		}

		vic_reg_writel(vic, VIC_RESOLUTION, (input_fmt->format.width << 16) |
				input_fmt->format.height);

		vic_reg_writel(vic, VIC_CONTROL_DELAY, (10 << 0| 10 << 16));
		vic_reg_writel(vic, VIC_CONTROL_TIZIANO_ROUTE, 1);
		vic_reg_writel(vic, VIC_CONTROL_DMA_ROUTE, 0);
		vic_reg_writel(vic, VIC_INT_MASK, 0x3ff7f);
		vic_reg_writel(vic, VIC_CONTROL, ~GLB_RST);
		vic_reg_writel(vic, VIC_CONTROL, REG_ENABLE);
#if 0
		unsigned int timeout  = 0xfffff;
		while(vic_reg_readl(vic,VIC_CONTROL) && --timeout);
		if(!timeout) {
			dev_err(vic->dev, "wait vic init timeout!\n");
		}
#endif

		vic_reg_writel(vic, VIC_CONTROL, VIC_START);


	} else {

		/* disable timestamp */
		vic_reg_writel(vic, VIC_TS_COUNTER, 0);
		vic_reg_writel(vic, VIC_TS_ENABLE, 0);

		/* keep reset*/
		vic_reg_writel(vic, VIC_CONTROL, GLB_RST);

		/* stop vic. */
	}

	return ret;
}


static const struct v4l2_subdev_video_ops vic_subdev_video_ops = {
        .s_stream = vic_subdev_s_stream,
};
static const struct v4l2_subdev_ops vic_subdev_ops = {
        .core = &vic_subdev_core_ops,
        .pad = &vic_subdev_pad_ops,
        .video = &vic_subdev_video_ops,
};

static ssize_t
dump_vic(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct vic_device *vic = dev_get_drvdata(dev);
	char *p = buf;

	p += sprintf(p, "VIC_CONTROL			:0x%08x\n", vic_reg_readl(vic, VIC_CONTROL));
	p += sprintf(p, "VIC_RESOLUTION                 :0x%08x\n", vic_reg_readl(vic, VIC_RESOLUTION));
	p += sprintf(p, "VIC_FRM_ECC                    :0x%08x\n", vic_reg_readl(vic, VIC_FRM_ECC));
	p += sprintf(p, "VIC_INTF_TYPE                  :0x%08x\n", vic_reg_readl(vic, VIC_INTF_TYPE));
	p += sprintf(p, "VIC_IN_DVP                     :0x%08x\n", vic_reg_readl(vic, VIC_IN_DVP));
	p += sprintf(p, "VIC_IN_CSI_FMT                 :0x%08x\n", vic_reg_readl(vic, VIC_IN_CSI_FMT));
	p += sprintf(p, "VIC_IN_HOR_PARA0               :0x%08x\n", vic_reg_readl(vic, VIC_IN_HOR_PARA0));
	p += sprintf(p, "VIC_IN_HOR_PARA1               :0x%08x\n", vic_reg_readl(vic, VIC_IN_HOR_PARA1));
	p += sprintf(p, "VIC_BK_CB_CTRL                 :0x%08x\n", vic_reg_readl(vic, VIC_BK_CB_CTRL));
	p += sprintf(p, "VIC_BK_CB_BLK                  :0x%08x\n", vic_reg_readl(vic, VIC_BK_CB_BLK));
	p += sprintf(p, "VIC_IN_VER_PARA0               :0x%08x\n", vic_reg_readl(vic, VIC_IN_VER_PARA0));
	p += sprintf(p, "VIC_IN_VER_PARA1               :0x%08x\n", vic_reg_readl(vic, VIC_IN_VER_PARA1));
	p += sprintf(p, "VIC_IN_VER_PARA2               :0x%08x\n", vic_reg_readl(vic, VIC_IN_VER_PARA2));
	p += sprintf(p, "VIC_IN_VER_PARA3               :0x%08x\n", vic_reg_readl(vic, VIC_IN_VER_PARA3));
	p += sprintf(p, "VIC_VLD_LINE_SAV               :0x%08x\n", vic_reg_readl(vic, VIC_VLD_LINE_SAV));
	p += sprintf(p, "VIC_VLD_LINE_EAV               :0x%08x\n", vic_reg_readl(vic, VIC_VLD_LINE_EAV));
	p += sprintf(p, "VIC_VLD_FRM_SAV                :0x%08x\n", vic_reg_readl(vic, VIC_VLD_FRM_SAV));
	p += sprintf(p, "VIC_VLD_FRM_EAV                :0x%08x\n", vic_reg_readl(vic, VIC_VLD_FRM_EAV));
	p += sprintf(p, "VIC_VC_CONTROL                 :0x%08x\n", vic_reg_readl(vic, VIC_VC_CONTROL));
	p += sprintf(p, "VIC_VC_CONTROL_CH0_PIX         :0x%08x\n", vic_reg_readl(vic, VIC_VC_CONTROL_CH0_PIX));
	p += sprintf(p, "VIC_VC_CONTROL_CH1_PIX         :0x%08x\n", vic_reg_readl(vic, VIC_VC_CONTROL_CH1_PIX));
	p += sprintf(p, "VIC_VC_CONTROL_CH2_PIX         :0x%08x\n", vic_reg_readl(vic, VIC_VC_CONTROL_CH2_PIX));
	p += sprintf(p, "VIC_VC_CONTROL_CH3_PIX         :0x%08x\n", vic_reg_readl(vic, VIC_VC_CONTROL_CH3_PIX));
	p += sprintf(p, "VIC_VC_CONTROL_CH0_LINE        :0x%08x\n", vic_reg_readl(vic, VIC_VC_CONTROL_CH0_LINE));
	p += sprintf(p, "VIC_VC_CONTROL_CH1_LINE        :0x%08x\n", vic_reg_readl(vic, VIC_VC_CONTROL_CH1_LINE));
	p += sprintf(p, "VIC_VC_CONTROL_CH2_LINE        :0x%08x\n", vic_reg_readl(vic, VIC_VC_CONTROL_CH2_LINE));
	p += sprintf(p, "VIC_VC_CONTROL_CH3_LINE        :0x%08x\n", vic_reg_readl(vic, VIC_VC_CONTROL_CH3_LINE));
	p += sprintf(p, "VIC_VC_CONTROL_FIFO_USE        :0x%08x\n", vic_reg_readl(vic, VIC_VC_CONTROL_FIFO_USE));
	p += sprintf(p, "VIC_CB_1ST                     :0x%08x\n", vic_reg_readl(vic, VIC_CB_1ST));
	p += sprintf(p, "VIC_CB_2ND                     :0x%08x\n", vic_reg_readl(vic, VIC_CB_2ND));
	p += sprintf(p, "VIC_CB_3RD                     :0x%08x\n", vic_reg_readl(vic, VIC_CB_3RD));
	p += sprintf(p, "VIC_CB_4TH                     :0x%08x\n", vic_reg_readl(vic, VIC_CB_4TH));
	p += sprintf(p, "VIC_CB_5TH                     :0x%08x\n", vic_reg_readl(vic, VIC_CB_5TH));
	p += sprintf(p, "VIC_CB_6TH                     :0x%08x\n", vic_reg_readl(vic, VIC_CB_6TH));
	p += sprintf(p, "VIC_CB_7TH                     :0x%08x\n", vic_reg_readl(vic, VIC_CB_7TH));
	p += sprintf(p, "VIC_CB_8TH                     :0x%08x\n", vic_reg_readl(vic, VIC_CB_8TH));
	p += sprintf(p, "VIC_CB2_1ST                    :0x%08x\n", vic_reg_readl(vic, VIC_CB2_1ST));
	p += sprintf(p, "VIC_CB2_2ND                    :0x%08x\n", vic_reg_readl(vic, VIC_CB2_2ND));
	p += sprintf(p, "VIC_CB2_3RD                    :0x%08x\n", vic_reg_readl(vic, VIC_CB2_3RD));
	p += sprintf(p, "VIC_CB2_4TH                    :0x%08x\n", vic_reg_readl(vic, VIC_CB2_4TH));
	p += sprintf(p, "VIC_CB2_5TH                    :0x%08x\n", vic_reg_readl(vic, VIC_CB2_5TH));
	p += sprintf(p, "VIC_CB2_6TH                    :0x%08x\n", vic_reg_readl(vic, VIC_CB2_6TH));
	p += sprintf(p, "VIC_CB2_7TH                    :0x%08x\n", vic_reg_readl(vic, VIC_CB2_7TH));
	p += sprintf(p, "VIC_CB2_8TH                    :0x%08x\n", vic_reg_readl(vic, VIC_CB2_8TH));
	p += sprintf(p, "MIPI_ALL_WIDTH_4BYTE           :0x%08x\n", vic_reg_readl(vic, MIPI_ALL_WIDTH_4BYTE));
	p += sprintf(p, "MIPI_VCROP_DEL01               :0x%08x\n", vic_reg_readl(vic, MIPI_VCROP_DEL01));
	p += sprintf(p, "MIPI_SENSOR_CONTROL            :0x%08x\n", vic_reg_readl(vic, MIPI_SENSOR_CONTROL));
	p += sprintf(p, "MIPI_HCROP_CH0                 :0x%08x\n", vic_reg_readl(vic, MIPI_HCROP_CH0));
	p += sprintf(p, "MIPI_VCROP_SHADOW_CFG          :0x%08x\n", vic_reg_readl(vic, MIPI_VCROP_SHADOW_CFG));
	p += sprintf(p, "VIC_CONTROL_LIMIT              :0x%08x\n", vic_reg_readl(vic, VIC_CONTROL_LIMIT));
	p += sprintf(p, "VIC_CONTROL_DELAY              :0x%08x\n", vic_reg_readl(vic, VIC_CONTROL_DELAY));
	p += sprintf(p, "VIC_CONTROL_TIZIANO_ROUTE      :0x%08x\n", vic_reg_readl(vic, VIC_CONTROL_TIZIANO_ROUTE));
	p += sprintf(p, "VIC_CONTROL_DMA_ROUTE          :0x%08x\n", vic_reg_readl(vic, VIC_CONTROL_DMA_ROUTE));
	p += sprintf(p, "VIC_INT_STA                    :0x%08x\n", vic_reg_readl(vic, VIC_INT_STA));
	p += sprintf(p, "VIC_INT_MASK                   :0x%08x\n", vic_reg_readl(vic, VIC_INT_MASK));
	p += sprintf(p, "VIC_INT_CLR                    :0x%08x\n", vic_reg_readl(vic, VIC_INT_CLR));
	p += sprintf(p, "VIC_DMA_CONFIG        :0x%08x\n", vic_reg_readl(vic, VIC_DMA_CONFIG));
	p += sprintf(p, "VIC_DMA_RESOLUTION    :0x%08x\n", vic_reg_readl(vic, VIC_DMA_RESOLUTION));
	p += sprintf(p, "VIC_DMA_RESET         :0x%08x\n", vic_reg_readl(vic, VIC_DMA_RESET));
	p += sprintf(p, "VIC_DMA_Y_STRID       :0x%08x\n", vic_reg_readl(vic, VIC_DMA_Y_STRID));
	p += sprintf(p, "VIC_DMA_Y_BUF0        :0x%08x\n", vic_reg_readl(vic, VIC_DMA_Y_BUF0));
	p += sprintf(p, "VIC_DMA_Y_BUF1        :0x%08x\n", vic_reg_readl(vic, VIC_DMA_Y_BUF1));
	p += sprintf(p, "VIC_DMA_Y_BUF2        :0x%08x\n", vic_reg_readl(vic, VIC_DMA_Y_BUF2));
	p += sprintf(p, "VIC_DMA_Y_BUF3        :0x%08x\n", vic_reg_readl(vic, VIC_DMA_Y_BUF3));
	p += sprintf(p, "VIC_DMA_Y_BUF4        :0x%08x\n", vic_reg_readl(vic, VIC_DMA_Y_BUF4));
	p += sprintf(p, "VIC_DMA_UV_STRID      :0x%08x\n", vic_reg_readl(vic, VIC_DMA_UV_STRID));
	p += sprintf(p, "VIC_DMA_UV_BUF0       :0x%08x\n", vic_reg_readl(vic, VIC_DMA_UV_BUF0));
	p += sprintf(p, "VIC_DMA_UV_BUF1       :0x%08x\n", vic_reg_readl(vic, VIC_DMA_UV_BUF1));
	p += sprintf(p, "VIC_DMA_UV_BUF2       :0x%08x\n", vic_reg_readl(vic, VIC_DMA_UV_BUF2));
	p += sprintf(p, "VIC_DMA_UV_BUF3       :0x%08x\n", vic_reg_readl(vic, VIC_DMA_UV_BUF3));
	p += sprintf(p, "VIC_DMA_UV_BUF4       :0x%08x\n", vic_reg_readl(vic, VIC_DMA_UV_BUF4));



	return p - buf;
}


static DEVICE_ATTR(dump_vic, S_IRUGO|S_IWUSR, dump_vic, NULL);

static struct attribute *vic_debug_attrs[] = {
        &dev_attr_dump_vic.attr,
	NULL,
};

static struct attribute_group vic_debug_attr_group = {
        .name   = "debug",
        .attrs  = vic_debug_attrs,
};

static int vic_comp_bind(struct device *comp, struct device *master,
                              void *master_data)
{
	struct vic_device *vic = dev_get_drvdata(comp);
	struct ispcam_device *ispcam = (struct ispcam_device *)master_data;
	struct v4l2_device *v4l2_dev = &ispcam->v4l2_dev;
	struct v4l2_subdev *sd = &vic->sd;
	int ret = 0;

	//dev_info(comp, "--------%s, %d \n", __func__, __LINE__);
	/* link subdev to master.*/
	vic->ispcam = (void *)ispcam;
	ispcam->vic = vic;

	v4l2_subdev_init(sd, &vic_subdev_ops);


	sd->owner = THIS_MODULE;
	sd->flags = V4L2_SUBDEV_FL_HAS_DEVNODE;
	strscpy(sd->name, dev_name(comp), sizeof(sd->name));
	v4l2_set_subdevdata(sd, vic);

	/* init vic pads. */
	vic->pads = kzalloc(sizeof(struct media_pad) * VIC_NUM_PADS, GFP_KERNEL);
	if(!vic->pads) {
		ret = -ENOMEM;
		goto err_alloc_pads;
	}
	vic->pads[0].index = VIC_PAD_SINK;
	vic->pads[0].flags = MEDIA_PAD_FL_SINK;	/* CSI->VIC, MIPI Interface*/
	vic->pads[1].index = VIC_PAD_SOURCE;
	vic->pads[1].flags = MEDIA_PAD_FL_SOURCE;

	ret = media_entity_init(&sd->entity, VIC_NUM_PADS, vic->pads, 0);

	/*3. register v4l2_subdev*/
	ret = v4l2_device_register_subdev(v4l2_dev, sd);
	if(ret < 0) {
		dev_err(comp, "Failed to register v4l2_subdev for vic\n");
		goto err_subdev_register;
	}
err_subdev_register:
err_alloc_pads:


	return ret;
}


static void vic_comp_unbind(struct device *comp, struct device *master,
                                 void *master_data)
{
        struct vic_device *vic = dev_get_drvdata(comp);

	dev_info(comp, "---TODO: %p-----%s, %d \n", vic, __func__, __LINE__);

}

static const struct component_ops vic_comp_ops = {
        .bind = vic_comp_bind,
        .unbind = vic_comp_unbind,
};


static int ingenic_vic_probe(struct platform_device *pdev)
{

	struct vic_device *vic = NULL;
	struct resource *regs = NULL;
	int ret = 0;

	vic = kzalloc(sizeof(struct vic_device), GFP_KERNEL);
	if(!vic) {
		pr_err("Failed to alloc vic dev [%s]\n", pdev->name);
		return -ENOMEM;
	}

	vic->dev = &pdev->dev;
	platform_set_drvdata(pdev, vic);


	ingenic_vic_parse_dt(vic);

	vic->irq = platform_get_irq(pdev, 0);
	if(vic->irq < 0) {
		dev_warn(&pdev->dev, "No CSI IRQ specified\n");
	}

	regs = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(!regs) {
		dev_err(&pdev->dev, "No iomem resource!\n");
		goto err_get_resource;
	}

	vic->iobase = devm_ioremap_resource(&pdev->dev, regs);
	if(!vic->iobase) {
		goto err_ioremap;
	}

	ret = sysfs_create_group(&vic->dev->kobj, &vic_debug_attr_group);
	if (ret) {
		dev_err(vic->dev, "device create sysfs group failed\n");

		ret = -EINVAL;
		goto err_sys_group;
	}

	ret = component_add(vic->dev, &vic_comp_ops);
	if(ret < 0) {
		dev_err(vic->dev, "Failed to add component vic!\n");
		goto err_component;
	}

	return 0;
err_component:
err_sys_group:
err_ioremap:
err_get_resource:
	return ret;
}



static int ingenic_vic_remove(struct platform_device *pdev)
{

	return 0;
}



static const struct of_device_id ingenic_vic_dt_match[] = {
        { .compatible = "ingenic,x2000-vic" },
        { }
};

MODULE_DEVICE_TABLE(of, ingenic_vic_dt_match);

static int __maybe_unused ingenic_vic_runtime_suspend(struct device *dev)
{
        return 0;
}

static int __maybe_unused ingenic_vic_runtime_resume(struct device *dev)
{
        return 0;
}

static const struct dev_pm_ops ingenic_vic_pm_ops = {
        SET_SYSTEM_SLEEP_PM_OPS(pm_runtime_force_suspend,
                                pm_runtime_force_resume)
        SET_RUNTIME_PM_OPS(ingenic_vic_runtime_suspend, ingenic_vic_runtime_resume, NULL)
};

static struct platform_driver ingenic_vic_driver = {
        .probe = ingenic_vic_probe,
        .remove = ingenic_vic_remove,
        .driver = {
                .name = "ingenic-vic",
                .of_match_table = ingenic_vic_dt_match,
                .pm = &ingenic_vic_pm_ops,
        },
};

module_platform_driver(ingenic_vic_driver);

MODULE_ALIAS("platform:ingenic-vic");
MODULE_DESCRIPTION("ingenic vic subsystem");
MODULE_AUTHOR("qipengzhen <aric.pzqi@ingenic.com>");
MODULE_LICENSE("GPL v2");

