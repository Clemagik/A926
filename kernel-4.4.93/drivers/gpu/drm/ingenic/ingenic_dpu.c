#include <drm/drmP.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_plane_helper.h>
#include <drm/drm_fourcc.h>
#include "ingenic_drv.h"
#include "dpu_reg.h"

#define to_drm_dev(x) (x->base.dev)

static void dump_dc_reg(struct ingenic_crtc *crtc)
{
	struct drm_device *dev = to_drm_dev(crtc);
	printk("-----------------dc_reg------------------\n");
	printk("DC_FRM_CFG_ADDR:    %lx\n",reg_read(dev, DC_FRM_CFG_ADDR));
	printk("DC_FRM_CFG_CTRL:    %lx\n",reg_read(dev, DC_FRM_CFG_CTRL));
	printk("DC_CTRL:            %lx\n",reg_read(dev, DC_CTRL));
	printk("DC_CSC_MULT_YRV:    %lx\n",reg_read(dev, DC_CSC_MULT_YRV));
	printk("DC_CSC_MULT_GUGV:   %lx\n",reg_read(dev, DC_CSC_MULT_GUGV));
	printk("DC_CSC_MULT_BU:     %lx\n",reg_read(dev, DC_CSC_MULT_BU));
	printk("DC_CSC_SUB_YUV:     %lx\n",reg_read(dev, DC_CSC_SUB_YUV));
	printk("DC_ST:              %lx\n",reg_read(dev, DC_ST));
	printk("DC_INTC:            %lx\n",reg_read(dev, DC_INTC));
	printk("DC_INT_FLAG:	    %lx\n",reg_read(dev, DC_INT_FLAG));
	printk("DC_COM_CONFIG:      %lx\n",reg_read(dev, DC_COM_CONFIG));
	printk("DC_PCFG_RD_CTRL:    %lx\n",reg_read(dev, DC_PCFG_RD_CTRL));
	printk("DC_OFIFO_PCFG:	    %lx\n",reg_read(dev, DC_OFIFO_PCFG));
	printk("DC_DISP_COM:        %lx\n",reg_read(dev, DC_DISP_COM));
	printk("-----------------dc_reg------------------\n");
}

static void dump_tft_reg(struct ingenic_crtc *crtc)
{
	struct drm_device *dev = to_drm_dev(crtc);
	printk("----------------tft_reg------------------\n");
	printk("TFT_TIMING_HSYNC:   %lx\n",reg_read(dev, DC_TFT_HSYNC));
	printk("TFT_TIMING_VSYNC:   %lx\n",reg_read(dev, DC_TFT_VSYNC));
	printk("TFT_TIMING_HDE:     %lx\n",reg_read(dev, DC_TFT_HDE));
	printk("TFT_TIMING_VDE:     %lx\n",reg_read(dev, DC_TFT_VDE));
	printk("TFT_TRAN_CFG:       %lx\n",reg_read(dev, DC_TFT_CFG));
	printk("TFT_ST:             %lx\n",reg_read(dev, DC_TFT_ST));
	printk("----------------tft_reg------------------\n");
}

static void dump_slcd_reg(struct ingenic_crtc *crtc)
{
	struct drm_device *dev = to_drm_dev(crtc);
	printk("---------------slcd_reg------------------\n");
	printk("SLCD_CFG:           %lx\n",reg_read(dev, DC_SLCD_CFG));
	printk("SLCD_WR_DUTY:       %lx\n",reg_read(dev, DC_SLCD_WR_DUTY));
	printk("SLCD_TIMING:        %lx\n",reg_read(dev, DC_SLCD_TIMING));
	printk("SLCD_FRM_SIZE:      %lx\n",reg_read(dev, DC_SLCD_FRM_SIZE));
	printk("SLCD_SLOW_TIME:     %lx\n",reg_read(dev, DC_SLCD_SLOW_TIME));
	printk("SLCD_CMD:           %lx\n",reg_read(dev, DC_SLCD_CMD));
	printk("SLCD_ST:            %lx\n",reg_read(dev, DC_SLCD_ST));
	printk("---------------slcd_reg------------------\n");
}

static void dump_frm_desc_reg(struct ingenic_crtc *crtc)
{
	struct drm_device *dev = to_drm_dev(crtc);
	unsigned int ctrl;
	ctrl = reg_read(dev, DC_CTRL);
	ctrl |= DC_DES_CNT_RST;
	reg_write(dev, DC_CTRL, ctrl);

	printk("--------Frame Descriptor register--------\n");
	printk("FrameNextCfgAddr:   %lx\n",reg_read(dev, DC_FRM_DES));
	printk("FrameSize:          %lx\n",reg_read(dev, DC_FRM_DES));
	printk("FrameCtrl:          %lx\n",reg_read(dev, DC_FRM_DES));
	printk("WritebackAddr:      %lx\n",reg_read(dev, DC_FRM_DES));
	printk("WritebackStride:    %lx\n",reg_read(dev, DC_FRM_DES));
	printk("Layer0CfgAddr:      %lx\n",reg_read(dev, DC_FRM_DES));
	printk("Layer1CfgAddr:      %lx\n",reg_read(dev, DC_FRM_DES));
	printk("Layer2CfgAddr:      %lx\n",reg_read(dev, DC_FRM_DES));
	printk("Layer3CfgAddr:      %lx\n",reg_read(dev, DC_FRM_DES));
	printk("LayCfgEn:	    %lx\n",reg_read(dev, DC_FRM_DES));
	printk("InterruptControl:   %lx\n",reg_read(dev, DC_FRM_DES));
	printk("--------Frame Descriptor register--------\n");
}

static void dump_layer_desc_reg(struct ingenic_crtc *crtc)
{
	struct drm_device *dev = to_drm_dev(crtc);
	unsigned int ctrl;
	ctrl = reg_read(dev, DC_CTRL);
	ctrl |= DC_DES_CNT_RST;
	reg_write(dev, DC_CTRL, ctrl);

	printk("--------layer0 Descriptor register-------\n");
	printk("LayerSize:          %lx\n",reg_read(dev, DC_LAY0_DES));
	printk("LayerCfg:           %lx\n",reg_read(dev, DC_LAY0_DES));
	printk("LayerBufferAddr:    %lx\n",reg_read(dev, DC_LAY0_DES));
	printk("LayerScale:         %lx\n",reg_read(dev, DC_LAY0_DES));
	printk("LayerRotation:      %lx\n",reg_read(dev, DC_LAY0_DES));
	printk("LayerScratch:       %lx\n",reg_read(dev, DC_LAY0_DES));
	printk("LayerPos:           %lx\n",reg_read(dev, DC_LAY0_DES));
	printk("LayerResizeCoef_X:  %lx\n",reg_read(dev, DC_LAY0_DES));
	printk("LayerResizeCoef_Y:  %lx\n",reg_read(dev, DC_LAY0_DES));
	printk("LayerStride:        %lx\n",reg_read(dev, DC_LAY0_DES));
	printk("--------layer0 Descriptor register-------\n");

	printk("--------layer1 Descriptor register-------\n");
	printk("LayerSize:          %lx\n",reg_read(dev, DC_LAY1_DES));
	printk("LayerCfg:           %lx\n",reg_read(dev, DC_LAY1_DES));
	printk("LayerBufferAddr:    %lx\n",reg_read(dev, DC_LAY1_DES));
	printk("LayerScale:         %lx\n",reg_read(dev, DC_LAY1_DES));
	printk("LayerRotation:      %lx\n",reg_read(dev, DC_LAY1_DES));
	printk("LayerScratch:       %lx\n",reg_read(dev, DC_LAY1_DES));
	printk("LayerPos:           %lx\n",reg_read(dev, DC_LAY1_DES));
	printk("LayerResizeCoef_X:  %lx\n",reg_read(dev, DC_LAY1_DES));
	printk("LayerResizeCoef_Y:  %lx\n",reg_read(dev, DC_LAY1_DES));
	printk("LayerStride:        %lx\n",reg_read(dev, DC_LAY1_DES));
	printk("--------layer1 Descriptor register-------\n");
}

static void dump_frm_desc(struct ingenic_crtc *crtc,
		struct ingenicfb_framedesc *framedesc,
		int index)
{
	printk("-------User Frame Descriptor index[%d]-----\n", index);
	printk("FramedescAddr:	    0x%x\n",(uint32_t)framedesc);
	printk("FrameNextCfgAddr:   0x%x\n",framedesc->FrameNextCfgAddr);
	printk("FrameSize:          0x%x\n",framedesc->FrameSize.d32);
	printk("FrameCtrl:          0x%x\n",framedesc->FrameCtrl.d32);
	printk("Layer0CfgAddr:      0x%x\n",framedesc->Layer0CfgAddr);
	printk("Layer1CfgAddr:      0x%x\n",framedesc->Layer1CfgAddr);
	printk("LayerCfgEn:	    0x%x\n",framedesc->LayCfgEn.d32);
	printk("InterruptControl:   0x%x\n",framedesc->InterruptControl.d32);
	printk("-------User Frame Descriptor index[%d]-----\n", index);
}

static void dump_layer_desc(struct ingenic_crtc *crtc,
		struct ingenicfb_layerdesc *layerdesc,
		int row, int col)
{
	printk("------User layer Descriptor index[%d][%d]------\n", row, col);
	printk("LayerdescAddr:	    0x%x\n",(uint32_t)layerdesc);
	printk("LayerSize:          0x%x\n",layerdesc->LayerSize.d32);
	printk("LayerCfg:           0x%x\n",layerdesc->LayerCfg.d32);
	printk("LayerBufferAddr:    0x%x\n",layerdesc->LayerBufferAddr);
	printk("LayerScale:         0x%x\n",layerdesc->LayerScale.d32);
	printk("LayerPos:           0x%x\n",layerdesc->LayerPos.d32);
	printk("LayerStride:        0x%x\n",layerdesc->LayerStride);
	printk("------User layer Descriptor index[%d][%d]------\n", row, col);
}

void dump_plane(struct ingenic_drm_plane * plane)
{
	printk("lay_en:		   0x%x\n",plane->lay_en);
	printk("lay_z_order:	   0x%x\n",plane->zpos);
	printk("pic_witdh:	   0x%x\n",plane->src_w);
	printk("pic_heght:	   0x%x\n",plane->src_h);
	printk("disp_pos_x:	   0x%x\n",plane->disp_pos_x);
	printk("disp_pos_y:	   0x%x\n",plane->disp_pos_y);
	printk("g_alpha_en:	   0x%x\n",plane->g_alpha_en);
	printk("g_alpha_val:	   0x%x\n",plane->g_alpha_val);
	printk("color:		   0x%x\n",plane->color);
	printk("format:		   0x%x\n",plane->format);
	printk("stride:		   0x%x\n",plane->stride);
}

static void dump_lcdc_registers(struct ingenic_crtc *crtc)
{
	dump_dc_reg(crtc);
	dump_tft_reg(crtc);
	dump_slcd_reg(crtc);
	dump_frm_desc_reg(crtc);
	dump_layer_desc_reg(crtc);
}

static void dump_desc(struct ingenic_crtc *crtc)
{
	int i, j;
	for(i = 0; i < MAX_DESC_NUM; i++) {
		for(j = 0; j < MAX_LAYER_NUM; j++) {
			dump_layer_desc(crtc, crtc->layerdesc[i][j], i, j);
		}
		dump_frm_desc(crtc, crtc->framedesc[i], i);
	}
}

void dump_all(struct ingenic_crtc *crtc)
{
	dump_lcdc_registers(crtc);
	dump_desc(crtc);
}

static int wait_dc_state(struct ingenic_crtc *crtc, uint32_t state, uint32_t flag)
{
	struct drm_device *dev = to_drm_dev(crtc);
	unsigned long timeout = 20000;
	while(((!(reg_read(dev, DC_ST) & state)) == flag) && timeout) {
		timeout--;
		udelay(10);
	}
	if(timeout <= 0) {
		printk("LCD wait state timeout! state = %d, DC_ST = 0x%x\n", state, DC_ST);
		return -1;
	}
	return 0;
}

static void composer_start(struct ingenic_crtc *crtc)
{
	struct drm_device *dev = to_drm_dev(crtc);

	if(!(reg_read(dev, DC_ST) & DC_FRM_WORKING)) {
		reg_write(dev, DC_FRM_CFG_CTRL, DC_FRM_START);
	}
}

static void composer_stop(struct ingenic_crtc *crtc)
{
	struct drm_device *dev = to_drm_dev(crtc);

	reg_write(dev, DC_CTRL, DC_QCK_STP_CMP);
	wait_dc_state(crtc, DC_WORKING, 0);
}

static void slcd_cfg_init(struct ingenic_crtc *crtc, struct drm_display_mode *mode) {
	struct smart_config *smart_config;
	struct drm_device *dev = to_drm_dev(crtc);
	uint32_t slcd_cfg;

	if(!mode->private) {
		printk("mode->private iS NULL\n");
		return;
	}
	smart_config = (struct smart_config *)mode->private;
	slcd_cfg = reg_read(dev, DC_SLCD_CFG);

	slcd_cfg |= DC_FRM_MD;

	if(smart_config->rdy_switch) {
		slcd_cfg |= DC_RDY_SWITCH;

		if(smart_config->rdy_dp) {
			slcd_cfg |= DC_RDY_DP;
		} else {
			slcd_cfg &= ~DC_RDY_DP;
		}
		if(smart_config->rdy_anti_jit) {
			slcd_cfg |= DC_RDY_ANTI_JIT;
		} else {
			slcd_cfg &= ~DC_RDY_ANTI_JIT;
		}
	} else {
		slcd_cfg &= ~DC_RDY_SWITCH;
	}

	if(smart_config->te_switch) {
		slcd_cfg |= DC_TE_SWITCH;

		if(smart_config->te_dp) {
			slcd_cfg |= DC_TE_DP;
		} else {
			slcd_cfg &= ~DC_TE_DP;
		}
		if(smart_config->te_md) {
			slcd_cfg |= DC_TE_MD;
		} else {
			slcd_cfg &= ~DC_TE_MD;
		}
		if(smart_config->te_anti_jit) {
			slcd_cfg |= DC_TE_ANTI_JIT;
		} else {
			slcd_cfg &= ~DC_TE_ANTI_JIT;
		}
	} else {
		slcd_cfg &= ~DC_TE_SWITCH;
	}

	if(smart_config->cs_en) {
		slcd_cfg |= DC_CS_EN;

		if(smart_config->cs_dp) {
			slcd_cfg |= DC_CS_DP;
		} else {
			slcd_cfg &= ~DC_CS_DP;
		}
	} else {
		slcd_cfg &= ~DC_CS_EN;
	}

	if(smart_config->dc_md) {
		slcd_cfg |= DC_DC_MD;
	} else {
		slcd_cfg &= ~DC_DC_MD;
	}

	if(smart_config->wr_md) {
		slcd_cfg |= DC_WR_DP;
	} else {
		slcd_cfg &= ~DC_WR_DP;
	}

	slcd_cfg &= ~DC_DBI_TYPE_MASK;
	switch(smart_config->smart_type){
	case SMART_LCD_TYPE_8080:
		slcd_cfg |= DC_DBI_TYPE_B_8080;
		break;
	case SMART_LCD_TYPE_6800:
		slcd_cfg |= DC_DBI_TYPE_A_6800;
		break;
	case SMART_LCD_TYPE_SPI_3:
		slcd_cfg |= DC_DBI_TYPE_C_SPI_3;
		break;
	case SMART_LCD_TYPE_SPI_4:
		slcd_cfg |= DC_DBI_TYPE_C_SPI_4;
		break;
	default:
		printk("err!\n");
		break;
	}

	slcd_cfg &= ~DC_DATA_FMT_MASK;
	switch(smart_config->pix_fmt) {
	case SMART_LCD_FORMAT_888:
		slcd_cfg |= DC_DATA_FMT_888;
		break;
	case SMART_LCD_FORMAT_666:
		slcd_cfg |= DC_DATA_FMT_666;
		break;
	case SMART_LCD_FORMAT_565:
		slcd_cfg |= DC_DATA_FMT_565;
		break;
	default:
		printk("err!\n");
		break;
	}

	slcd_cfg &= ~DC_DWIDTH_MASK;
	switch(smart_config->dwidth) {
	case SMART_LCD_DWIDTH_8_BIT:
		slcd_cfg |= DC_DWIDTH_8BITS;
		break;
	case SMART_LCD_DWIDTH_9_BIT:
		slcd_cfg |= DC_DWIDTH_9BITS;
		break;
	case SMART_LCD_DWIDTH_16_BIT:
		slcd_cfg |= DC_DWIDTH_16BITS;
		break;
	case SMART_LCD_DWIDTH_18_BIT:
		slcd_cfg |= DC_DWIDTH_18BITS;
		break;
	case SMART_LCD_DWIDTH_24_BIT:
		slcd_cfg |= DC_DWIDTH_24BITS;
		break;
	default:
		printk("err!\n");
		break;
	}

	slcd_cfg &= ~DC_CWIDTH_MASK;
	switch(smart_config->cwidth) {
	case SMART_LCD_CWIDTH_8_BIT:
		slcd_cfg |= DC_CWIDTH_8BITS;
		break;
	case SMART_LCD_CWIDTH_9_BIT:
		slcd_cfg |= DC_CWIDTH_9BITS;
		break;
	case SMART_LCD_CWIDTH_16_BIT:
		slcd_cfg |= DC_CWIDTH_16BITS;
		break;
	case SMART_LCD_CWIDTH_18_BIT:
		slcd_cfg |= DC_CWIDTH_18BITS;
		break;
	case SMART_LCD_CWIDTH_24_BIT:
		slcd_cfg |= DC_CWIDTH_24BITS;
		break;
	default:
		printk("err!\n");
		break;
	}

	reg_write(dev, DC_SLCD_CFG, slcd_cfg);

	return;
}

static int slcd_timing_init(struct ingenic_crtc *crtc, struct drm_display_mode *mode)
{
	struct drm_device *dev = to_drm_dev(crtc);
	uint32_t width = mode->hdisplay;
	uint32_t height = mode->vdisplay;
	uint32_t dhtime = 0;
	uint32_t dltime = 0;
	uint32_t chtime = 0;
	uint32_t cltime = 0;
	uint32_t tah = 0;
	uint32_t tas = 0;
	uint32_t slowtime = 0;

	/*frm_size*/
	reg_write(dev, DC_SLCD_FRM_SIZE,
		  ((width << DC_SLCD_FRM_H_SIZE_LBIT) |
		   (height << DC_SLCD_FRM_V_SIZE_LBIT)));

	/* wr duty */
	reg_write(dev, DC_SLCD_WR_DUTY,
		  ((dhtime << DC_DSTIME_LBIT) |
		   (dltime << DC_DDTIME_LBIT) |
		   (chtime << DC_CSTIME_LBIT) |
		   (cltime << DC_CDTIME_LBIT)));

	/* slcd timing */
	reg_write(dev, DC_SLCD_TIMING,
		  ((tah << DC_TAH_LBIT) |
		  (tas << DC_TAS_LBIT)));

	/* slow time */
	reg_write(dev, DC_SLCD_SLOW_TIME, slowtime);

	return 0;
}

static void slcd_send_mcu_cmd(struct drm_device *dev, unsigned long cmd)
{
	int count = 10000;
	uint32_t slcd_cfg;

	while ((reg_read(dev, DC_SLCD_ST) & DC_SLCD_ST_BUSY) && count--) {
		udelay(10);
	}
	if (count < 0) {
		dev_err(dev->dev, "SLCDC wait busy state wrong");
	}

	slcd_cfg = reg_read(dev, DC_SLCD_CFG);
	reg_write(dev, DC_SLCD_CMD, DC_SLCD_CMD_FLAG_CMD | (cmd & ~DC_SLCD_CMD_FLAG_MASK));
}

static void slcd_send_mcu_data(struct drm_device *dev, unsigned long data)
{
	int count = 10000;
	uint32_t slcd_cfg;

	while ((reg_read(dev, DC_SLCD_ST) & DC_SLCD_ST_BUSY) && count--) {
		udelay(10);
	}
	if (count < 0) {
		dev_err(dev->dev, "SLCDC wait busy state wrong");
	}

	slcd_cfg = reg_read(dev, DC_SLCD_CFG);
	reg_write(dev, DC_SLCD_CFG, (slcd_cfg | DC_FMT_EN));
	reg_write(dev, DC_SLCD_CMD, DC_SLCD_CMD_FLAG_DATA | (data & ~DC_SLCD_CMD_FLAG_MASK));
}

static void slcd_send_mcu_prm(struct drm_device *dev, unsigned long data)
{
	int count = 10000;
	uint32_t slcd_cfg;

	while ((reg_read(dev, DC_SLCD_ST) & DC_SLCD_ST_BUSY) && count--) {
		udelay(10);
	}
	if (count < 0) {
		dev_err(dev->dev, "SLCDC wait busy state wrong");
	}

	slcd_cfg = reg_read(dev, DC_SLCD_CFG);
	reg_write(dev, DC_SLCD_CMD, DC_SLCD_CMD_FLAG_PRM | (data & ~DC_SLCD_CMD_FLAG_MASK));
}

static void slcd_mcu_init(struct ingenic_crtc *crtc, struct drm_display_mode *mode)
{
	struct smart_config *smart_config;
	struct drm_device *dev = to_drm_dev(crtc);
	struct smart_lcd_data_table *data_table;
	uint32_t length_data_table;
	uint32_t i;

	if(!mode->private) {
		printk("mode->private iS NULL\n");
		return;
	}
	smart_config = (struct smart_config *)mode->private;
	data_table = smart_config->data_table;
	length_data_table = smart_config->length_data_table;

	if(length_data_table && data_table) {
		for(i = 0; i < length_data_table; i++) {
			switch (data_table[i].type) {
			case SMART_CONFIG_DATA:
				slcd_send_mcu_data(dev, data_table[i].value);
				break;
			case SMART_CONFIG_PRM:
				slcd_send_mcu_prm(dev, data_table[i].value);
				break;
			case SMART_CONFIG_CMD:
				slcd_send_mcu_cmd(dev, data_table[i].value);
				break;
			case SMART_CONFIG_UDELAY:
				udelay(data_table[i].value);
				break;
			default:
				printk("Unknow SLCD data type\n");
				break;
			}
		}
	}
}

static void slcd_set_par(struct ingenic_crtc *crtc,
		struct drm_display_mode *mode)
{
	slcd_mcu_init(crtc, mode);
	slcd_cfg_init(crtc, mode);
	slcd_timing_init(crtc, mode);
}

static void wait_slcd_busy(struct ingenic_crtc *crtc)
{
	struct drm_device *dev = to_drm_dev(crtc);
	int count = 100000;
	while ((reg_read(dev, DC_SLCD_ST) & DC_SLCD_ST_BUSY)
			&& count--) {
		udelay(10);
	}
	if (count < 0) {
		dev_err(dev->dev,"SLCDC wait busy state wrong");
	}
}

static void slcd_mode_set(struct ingenic_crtc *crtc,
				struct drm_display_mode *mode)
{
	struct drm_device *dev = to_drm_dev(crtc);
	unsigned int disp_com;

	disp_com = reg_read(dev, DC_DISP_COM);
	disp_com &= ~DC_DP_IF_SEL;
	reg_write(dev, DC_DISP_COM, disp_com | DC_DISP_COM_SLCD);
	slcd_set_par(crtc, mode);
}

static void slcd_enable(struct ingenic_crtc *crtc)
{
	struct drm_device *dev = to_drm_dev(crtc);

	composer_start(crtc);

	slcd_send_mcu_cmd(dev, 0x2c);
	wait_slcd_busy(crtc);

	if(!(reg_read(dev, DC_SLCD_ST) & DC_SLCD_ST_BUSY)) {
		reg_write(dev, DC_CTRL, DC_SLCD_START);
	} else {
		dev_err(dev->dev, "Slcd has enabled.\n");
	}
}

static void tft_set_par(struct ingenic_crtc *crtc,
		struct drm_display_mode *mode)
{
	struct drm_device *dev = to_drm_dev(crtc);
	uint32_t hps;
	uint32_t hpe;
	uint32_t vps;
	uint32_t vpe;
	uint32_t hds;
	uint32_t hde;
	uint32_t vds;
	uint32_t vde;
	uint32_t tft_cfg = 0;

	hps = mode->hsync_end - mode->hsync_start;
	hpe = mode->htotal;
	vps = mode->vsync_end - mode->vsync_start;
	vpe = mode->vtotal;

	hds = mode->htotal - mode->hsync_start;
	hde = hds + mode->hdisplay;
	vds = mode->vtotal - mode->vsync_start;
	vde = vds + mode->vdisplay;

	reg_write(dev, DC_TFT_HSYNC,
		  (hps << DC_HPS_LBIT) |
		  (hpe << DC_HPE_LBIT));
	reg_write(dev, DC_TFT_VSYNC,
		  (vps << DC_VPS_LBIT) |
		  (vpe << DC_VPE_LBIT));
	reg_write(dev, DC_TFT_HDE,
		  (hds << DC_HDS_LBIT) |
		  (hde << DC_HDE_LBIT));
	reg_write(dev, DC_TFT_VDE,
		  (vds << DC_VDS_LBIT) |
		  (vde << DC_VDE_LBIT));

	tft_cfg &= ~DC_PIX_CLK_INV;
	tft_cfg &= ~DC_DE_DL;
	tft_cfg &= ~DC_SYNC_DL;
	tft_cfg &= ~DC_COLOR_EVEN_MASK;
	tft_cfg |= DC_EVEN_RGB;
	tft_cfg &= ~DC_COLOR_ODD_MASK;
	tft_cfg |= DC_ODD_RGB;
	tft_cfg &= ~DC_MODE_MASK;
	tft_cfg |= DC_MODE_PARALLEL_666;
	reg_write(dev, DC_TFT_CFG, tft_cfg);
}

static void tft_enable(struct ingenic_crtc *crtc)
{
	struct drm_device *dev = to_drm_dev(crtc);

	composer_start(crtc);
	if(!(reg_read(dev, DC_TFT_ST) & DC_TFT_WORKING)) {
		reg_write(dev, DC_CTRL, DC_TFT_START);
	} else {
		dev_err(dev->dev, "TFT has enabled.\n");
	}
}

static void tft_mode_set(struct ingenic_crtc *crtc,
				struct drm_display_mode *mode)
{
	struct drm_device *dev = to_drm_dev(crtc);
	unsigned int disp_com;

	disp_com = reg_read(dev, DC_DISP_COM);
	disp_com &= ~DC_DP_IF_SEL;
	reg_write(dev, DC_DISP_COM, disp_com | DC_DISP_COM_TFT);
	tft_set_par(crtc, mode);
}

static void dpu_common_mode_set(struct ingenic_crtc *crtc)
{
	struct drm_device *dev = to_drm_dev(crtc);
	unsigned com_cfg = 0;
	uint32_t disp_com;

	disp_com = reg_read(dev, DC_DISP_COM);
	if(crtc->dither_enable) {
		disp_com |= DC_DP_DITHER_EN;
		disp_com &= ~DC_DP_DITHER_DW_MASK;
		disp_com |= crtc->dither.dither_red
			     << DC_DP_DITHER_DW_RED_LBIT;
		disp_com |= crtc->dither.dither_green
			    << DC_DP_DITHER_DW_GREEN_LBIT;
		disp_com |= crtc->dither.dither_blue
			    << DC_DP_DITHER_DW_BLUE_LBIT;
	} else {
		disp_com &= ~DC_DP_DITHER_EN;
	}
	reg_write(dev, DC_DISP_COM, disp_com);

	com_cfg = reg_read(dev, DC_COM_CONFIG);
	com_cfg &= ~DC_OUT_SEL;
	/* set burst length 32*/
	com_cfg &= ~DC_BURST_LEN_BDMA_MASK;
	com_cfg |= DC_BURST_LEN_BDMA_32;
	reg_write(dev, DC_COM_CONFIG, com_cfg);

	reg_write(dev, DC_CLR_ST, 0x01FFFFFE);

	reg_write(dev, DC_FRM_CFG_ADDR, crtc->framedesc_phys[0]);
}

struct dpu_dp_ops slcd_dp_ops = {
	.enable = slcd_enable,
	.mode_set = slcd_mode_set,
	.disable = composer_stop,
	.common_mode_set = dpu_common_mode_set,
};

struct dpu_dp_ops tft_dp_ops = {
	.enable = tft_enable,
	.mode_set = tft_mode_set,
	.disable = composer_stop,
	.common_mode_set = dpu_common_mode_set,
};

static void dpu_enable(struct ingenic_crtc *crtc)
{
	if(crtc->enable)
		return;
	crtc->dp_ops->enable(crtc);
	crtc->enable = true;
}
static void dpu_disable(struct ingenic_crtc *crtc)
{
	if(!crtc->enable)
		return;
	crtc->dp_ops->disable(crtc);
	crtc->enable = false;
}
static void dpu_enable_vblank(struct ingenic_crtc *crtc, bool enable)
{
	struct drm_device *dev = to_drm_dev(crtc);
	u32 tmp;

	tmp = reg_read(dev, DC_INTC);
	if (enable) {
		reg_write(dev, DC_CLR_ST, DC_CLR_DISP_END);
		reg_write(dev, DC_INTC, tmp | DC_EOD_MSK);
	} else {
		reg_write(dev, DC_INTC, tmp & ~DC_EOD_MSK);
	}
}

static void dpu_mode_set(struct ingenic_crtc *crtc,
			struct drm_display_mode *mode)
{
	bool enable;

	if(crtc->enable) {
		dpu_disable(crtc);
		enable = true;
	}
	crtc->dp_ops->common_mode_set(crtc);
	crtc->dp_ops->mode_set(crtc, mode);
	if(enable)
		dpu_enable(crtc);
}
static void dpu_disable_plane(struct ingenic_crtc *crtc,
		      struct ingenic_drm_plane *plane)
{
	struct ingenicfb_framedesc **framedesc;
	int frm_num_mi, frm_num_ma;
	int i;

	framedesc = crtc->framedesc;

	frm_num_mi = 0;
	frm_num_ma = MAX_DESC_NUM;
	for(i = frm_num_mi; i < frm_num_ma; i++)
		plane->lay_en = 0;
}

static void dpu_update_plane(struct ingenic_crtc *crtc)
{
	struct drm_display_mode *mode = &crtc->base.state->adjusted_mode;
	struct ingenicfb_framedesc **framedesc;
	struct ingenic_drm_plane *plane = crtc->plane;
	int frm_num_mi, frm_num_ma;
	int i, j;

	framedesc = crtc->framedesc;

	frm_num_mi = 0;
	frm_num_ma = 1;

	for(i = frm_num_mi; i < frm_num_ma; i++) {
		for(j =0; j < MAX_LAYER_NUM; j++) {
			if(!plane[j].lay_en)
				continue;
			crtc->layerdesc[i][j]->LayerSize.b.width = plane[j].src_w;
			crtc->layerdesc[i][j]->LayerSize.b.height = plane[j].src_h;
			crtc->layerdesc[i][j]->LayerPos.b.x_pos = plane[j].disp_pos_x;
			crtc->layerdesc[i][j]->LayerPos.b.y_pos = plane[j].disp_pos_y;
			crtc->layerdesc[i][j]->LayerCfg.b.g_alpha_en = plane[j].g_alpha_en;
			crtc->layerdesc[i][j]->LayerCfg.b.g_alpha = plane[j].g_alpha_val;
			crtc->layerdesc[i][j]->LayerCfg.b.color = plane[j].color;
			crtc->layerdesc[i][j]->LayerCfg.b.domain_multi = 1;
			crtc->layerdesc[i][j]->LayerCfg.b.format = plane[j].format;
			crtc->layerdesc[i][j]->LayerStride = plane[j].stride;
			crtc->layerdesc[i][j]->LayerBufferAddr = plane[j].addr_offset[0];
		}
	}

	for(i = frm_num_mi; i < frm_num_ma; i++) {
		framedesc[i]->FrameNextCfgAddr = crtc->framedesc_phys[i];
		framedesc[i]->FrameSize.b.width = mode->hdisplay;
		framedesc[i]->FrameSize.b.height = mode->vdisplay;
		framedesc[i]->FrameCtrl.d32 = FRAME_CTRL_DEFAULT_SET;
		framedesc[i]->Layer0CfgAddr = crtc->layerdesc_phys[i][0];
		framedesc[i]->Layer1CfgAddr = crtc->layerdesc_phys[i][1];
		framedesc[i]->LayCfgEn.b.lay0_en = plane[0].lay_en;
		framedesc[i]->LayCfgEn.b.lay1_en = plane[1].lay_en;
		framedesc[i]->LayCfgEn.b.lay0_z_order = 1;
		framedesc[i]->LayCfgEn.b.lay1_z_order = 0;
		framedesc[i]->FrameCtrl.b.stop = 0;
		framedesc[i]->InterruptControl.d32 = DC_EOD_MSK;
	}
}

static irqreturn_t dpu_irq_handler(struct ingenic_crtc *crtc)
{
	struct drm_device *dev = to_drm_dev(crtc);
	unsigned int irq_flag;

	irq_flag = reg_read(dev, DC_INT_FLAG);
	if(likely(irq_flag & DC_DISP_END)) {
		bool put_vblank = false;
		unsigned long flags;
		crtc->frm_end++;
		spin_lock_irqsave(&dev->event_lock, flags);
		spin_unlock_irqrestore(&dev->event_lock, flags);
		drm_handle_vblank(dev, 0);
		if (put_vblank)
			drm_vblank_put(dev, 0);
		reg_write(dev, DC_CLR_ST, DC_CLR_FRM_START);
		return IRQ_HANDLED;
	}

	dev_err(dev->dev, "DPU irq nothing do, please check!!!\n");
	return IRQ_HANDLED;
}

struct ingenic_dpu_ops dpu_ops = {
	.enable		= dpu_enable,
	.disable	= dpu_disable,
	.enable_vblank	= dpu_enable_vblank,
	.mode_set	= dpu_mode_set,
	.disable_plane	= dpu_disable_plane,
	.update_plane	= dpu_update_plane,
	.irq_handler	= dpu_irq_handler,
};

int dpu_register(struct ingenic_crtc *crtc)
{
	if(crtc->lcd_type == LCD_TYPE_SLCD)
		crtc->dp_ops = &slcd_dp_ops;
	else
		crtc->dp_ops = &tft_dp_ops;
	crtc->dpu_ops = &dpu_ops;
	return 0;
}
