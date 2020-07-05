#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/slab.h>
#if 0
#include "../inc/system_sensor_drv.h"
#include "../inc/tiziano_sys.h"
#include "../inc/tiziano_core_tuning.h"
#endif
#include "../inc/tiziano_core.h"

#include "tiziano_map.h"
#include "tiziano_adr.h"
#include "../inc/tiziano_isp.h"
#include "tiziano_core.h"
#include "tiziano_event.h"
#include "../inc/tiziano_sys.h"
#include "tiziano_ae.h"
#include "tiziano_awb.h"
#include "tiziano_gamma.h"
#include "tiziano_gib.h"
#include "tiziano_lsc.h"
#include "tiziano_ccm.h"
#include "tiziano_dmsc.h"
#include "tiziano_sharpen.h"
#include "tiziano_sdns.h"
#include "tiziano_mdns.h"
#include "tiziano_clm.h"
#include "tiziano_dpc.h"
#include "tiziano_defog.h"
#include "tiziano_adr.h"
#include "tiziano_hldc.h"
#include "tiziano_af.h"
#include "tiziano_params_operate.h"

#if 0
tisp_info_t tispinfo;
sensor_control_t sensor_ctrl;
tisp_custom_effect_t custom_eff;
tisp_mdns_ratio_t mdns_ratio;
int deir_en = 0;
int ae_switch = 0;
uint32_t topreg_val = 0x3f00;
tisp_init_param_t sensor_info;
int tiziano_load_parameters(void);
#endif

int tiziano_load_parameters(tisp_core_t *core);


static void dump_one_register(unsigned int reg)
{
	unsigned int val;
	val = *(volatile unsigned int *)(reg);
	printk("[REG %x]:%x\n", reg, val);
}

static void __maybe_unused dump_all_register(void)
{
	/*vic*/
	unsigned int reg = 0;
	printk("*****************VIC**********\n");
	for(reg = 0xb3710000; reg < 0xb37101ff; reg+=4)
		dump_one_register(reg);
	/*top*/
	printk("******************TOP***********\n");
	for(reg = 0xb3700000; reg < 0xb3700100; reg+=4)
		dump_one_register(reg);
	/*input*/
	printk("******************input***********\n");
	for(reg = 0xb3700100; reg < 0xb3700200; reg+=4)
		dump_one_register(reg);
	/*mscaler*/
	printk("******************mscaler***********\n");
	for(reg = 0xb3702300; reg < 0xb37027ff; reg+=4)
		dump_one_register(reg);
#if 0
	/*AG*/
	printk("******************AG***********\n");
	for(reg = 0xb3700600; reg < 0xb37006ff; reg+=4)
		dump_one_register(reg);
	/*AE*/
	printk("******************AE***********\n");
	for(reg = 0xb3700700; reg < 0xb37007ff; reg+=4)
		dump_one_register(reg);

	/*AF*/
	printk("******************AF***********\n");
	for(reg = 0xb3700900; reg < 0xb37009ff; reg+=4)
		dump_one_register(reg);

	/*IRINT*/
	printk("******************IRINT***********\n");
	for(reg = 0xb3700f00; reg < 0xb3700fff; reg+=4)
		dump_one_register(reg);
	/*CSC*/
	printk("******************SCS***********\n");
	for(reg = 0xb3701200; reg < 0xb37012ff; reg+=4)
		dump_one_register(reg);
#endif
}
int32_t tisp_tgain_update(void *cb_data, uint64_t data1, uint64_t data2, uint64_t data3, uint64_t data4)
{
	tisp_core_t *core = (tisp_core_t *)cb_data;

	tisp_dmsc_refresh(&core->dmsc, data1);
	tisp_sharpen_refresh(&core->sharpen, data1);
	tisp_sdns_refresh(&core->sdns, data1);
	tisp_dpc_refresh(&core->dpc, data1);
	tisp_mdns_refresh(&core->mdns, data1);
	tisp_lsc_gain_update(&core->lsc, data1);

	/* ISP_ERROR("%s:%d-------gain value is %llx\n",data1); */

	return 0;
}

int32_t tisp_again_update(void *cb_data, uint64_t data1, uint64_t data2, uint64_t data3, uint64_t data4)
{
	tisp_core_t *core = (tisp_core_t *)cb_data;
	/* ISP_ERROR("%s,%d: tgain_db = 0x%x\n", __func__, __LINE__, (uint32_t)data1); */
	/* ISP_ERROR("%s:%d-------gain value is %llx\n",data1); */
	tisp_gib_gain_interpolation(&core->gib, data1);

	return 0;
}

static uint64_t ev_last = 0;
int32_t tisp_ev_update(void *cb_data, uint64_t data1, uint64_t data2, uint64_t data3, uint64_t data4)
{
	tisp_core_t *core = (tisp_core_t *)cb_data;

	/* ISP_ERROR("%s,%d: ev = 0x%llx\n", __func__, __LINE__, data1); */
	if(data1 != ev_last){
		tisp_awb_ev_update(&core->awb, data1);
		tisp_ccm_ev_update(&core->ccm, data1);
		tisp_adr_ev_update(&core->adr, data1);
		tisp_defog_ev_update(&core->defog, data1);
	}
	return 0;
}

int32_t tisp_ct_update(void *cb_data, uint64_t data1, uint64_t data2, uint64_t data3, uint64_t data4)
{
	tisp_core_t *core = (tisp_core_t *)cb_data;

	tisp_ccm_ct_update(&core->ccm, data1);
	tisp_lsc_ct_update(&core->lsc, data1);

	return 0;
}

int32_t tisp_ae_ir_update(void *cb_data, uint64_t data1, uint64_t data2, uint64_t data3, uint64_t data4)
{
	tisp_core_t *core = (tisp_core_t *)cb_data;

	tisp_gib_deir_ir_update(&core->gib, data1);

	return 0;
}

int tisp_core_init(tisp_core_t *core, tisp_init_param_t *p, void *priv_data)
{
	int ret = 0;
	unsigned int vaddr;
	unsigned int paddr;
	uint32_t width;
	uint32_t height;
	tisp_ae_ex_min_t ae_init;
	int i = 0;
	unsigned int top_bypass = TIZIANO_BYPASS_INIT;
	tisp_info_t *tispinfo 			= NULL;
	tisp_init_param_t *sensor_info 		= NULL;
	tisp_custom_effect_t *custom_eff	= NULL;
	tisp_mdns_ratio_t *mdns_ratio		= NULL;
	sensor_control_t *sensor_ctrl 		= NULL;


	memset(core, 0, sizeof(tisp_core_t));

	tispinfo 		= &core->tispinfo;
	sensor_info 		= &core->sensor_info;
	custom_eff		= &core->custom_eff;
	mdns_ratio		= &core->mdns_ratio;
	sensor_ctrl 		= &core->sensor_ctrl;
	core->topreg_val			= 0x3f00;

	core->priv_data = priv_data;

	core->awb.core 		= core;
	core->gamma.core 	= core;
	core->defog.core 	= core;
	core->adr.core 		= core;
	core->ae.core 		= core;
	core->af.core 		= core;
 	core->gib.core		= core;
 	core->dmsc.core		= core;
 	core->lsc.core		= core;
 	core->ccm.core		= core;
 	core->sharpen.core	= core;
 	core->dpc.core		= core;
 	core->sdns.core		= core;
 	core->mdns.core		= core;
 	core->clm.core		= core;
 	core->hldc.core		= core;
 	core->param_operate.core= core;
	core->core_tuning.core  = core;
	core->tisp_event_mg.core = core;


	memset(tispinfo, 0, sizeof(tispinfo));
	memset(sensor_info, 0, sizeof(sensor_info));

	memcpy(sensor_info, p, sizeof(sensor_info));
	ret = tiziano_load_parameters(core);
	if (ret != 0)
		ISP_ERROR("no bin file on the system!!!\n");
	/*TODO:*/
	memcpy(&tparams, &tparams_day, sizeof(tparams));

	/* init brightness/sharpness/contrast/saturation */
	memset(custom_eff,128,sizeof(custom_eff));

	/* Brightness */
	memcpy(custom_eff->brightness_ori_day,
	       tparams_day.params_data.TISP_PARAM_AE_AT_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_AE_AT_LIST));
	memcpy(&custom_eff->brightness_ori_day[10],
	       tparams_day.params_data.TISP_PARAM_AE_EV_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_AE_EV_LIST));
	memcpy(custom_eff->brightness_ori_night,
	       tparams_night.params_data.TISP_PARAM_AE_AT_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_AE_AT_LIST));
	memcpy(&custom_eff->brightness_ori_night[10],
	       tparams_night.params_data.TISP_PARAM_AE_EV_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_AE_EV_LIST));
	/* Saturation */
	memcpy(custom_eff->saturation_ori_day,
	       tparams_day.params_data.TISP_PARAM_CCM_SAT_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_CCM_SAT_LIST));
	/* Sharpen */
	memcpy(custom_eff->sharpen_ori_day,
	       tparams_day.params_data.TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY));
	memcpy(&custom_eff->sharpen_ori_day[9],
	       tparams_day.params_data.TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY));
	memcpy(&custom_eff->sharpen_ori_day[18],
	       tparams_day.params_data.TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY));
	memcpy(&custom_eff->sharpen_ori_day[27],
	       tparams_day.params_data.TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY));

	memcpy(custom_eff->sharpen_ori_night,
	       tparams_night.params_data.TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY));
	memcpy(&custom_eff->sharpen_ori_night[9],
	       tparams_night.params_data.TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY));
	memcpy(&custom_eff->sharpen_ori_night[18],
	       tparams_night.params_data.TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY));
	memcpy(&custom_eff->sharpen_ori_night[27],
	       tparams_night.params_data.TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY));

	/* save init mdns parameters */
	memset(mdns_ratio,128,sizeof(tisp_mdns_ratio_t));
	memcpy(mdns_ratio->dysad_thres_def_day,
	       tparams_day.params_data.TISP_PARAM_MDSN_Y_SAD_THRES_ARRAY,
	       sizeof(tparams_day.params_data.TISP_PARAM_MDSN_Y_SAD_THRES_ARRAY));
	memcpy(mdns_ratio->dysta_thres_def_day,
	       tparams_day.params_data.TISP_PARAM_MDSN_Y_STA_THRES_ARRAY,
	       sizeof(tparams_day.params_data.TISP_PARAM_MDSN_Y_STA_THRES_ARRAY));
	memcpy(mdns_ratio->dypbt_thres_def_day,
	       tparams_day.params_data.TISP_PARAM_MDSN_Y_PBT_THRES_ARRAY,
	       sizeof(tparams_day.params_data.TISP_PARAM_MDSN_Y_PBT_THRES_ARRAY));
	memcpy(mdns_ratio->dywei_max_def_day,
	       tparams_day.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MAX_ARRAY,
	       sizeof(tparams_day.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MAX_ARRAY));
	memcpy(mdns_ratio->dywei_min_def_day,
	       tparams_day.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MIN_ARRAY,
	       sizeof(tparams_day.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MIN_ARRAY));
	memcpy(mdns_ratio->dysad_thres_def_night,
	       tparams_night.params_data.TISP_PARAM_MDSN_Y_SAD_THRES_ARRAY,
	       sizeof(tparams_night.params_data.TISP_PARAM_MDSN_Y_SAD_THRES_ARRAY));
	memcpy(mdns_ratio->dysta_thres_def_night,
	       tparams_night.params_data.TISP_PARAM_MDSN_Y_STA_THRES_ARRAY,
	       sizeof(tparams_night.params_data.TISP_PARAM_MDSN_Y_STA_THRES_ARRAY));
	memcpy(mdns_ratio->dypbt_thres_def_night,
	       tparams_night.params_data.TISP_PARAM_MDSN_Y_PBT_THRES_ARRAY,
	       sizeof(tparams_night.params_data.TISP_PARAM_MDSN_Y_PBT_THRES_ARRAY));
	memcpy(mdns_ratio->dywei_max_def_night,
	       tparams_night.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MAX_ARRAY,
	       sizeof(tparams_night.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MAX_ARRAY));
	memcpy(mdns_ratio->dywei_min_def_night,
	       tparams_night.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MIN_ARRAY,
	       sizeof(tparams_night.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MIN_ARRAY));

	/* frame size */
	system_reg_write(core->priv_data, TIZIANO_FM_SIZE, (p->width << 16 | p->height));
	tispinfo->sensor_width = p->width;
	tispinfo->sensor_height = p->height;
	width = tispinfo->sensor_width;
	height = tispinfo->sensor_height;

	/* bayer type */
	switch(p->bayer){
	case 0://RGGB
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0x0);
		core->deir_en = 0;
		break;
	case 1://GRBG
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0x2);
		core->deir_en = 0;
		break;
	case 2://BGGR
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0x1);
		core->deir_en = 0;
		break;
	case 3://GBRG
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0x3);
		core->deir_en = 0;
		break;
	case 4://RGIRB
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0x8);
		core->deir_en = 1;
		break;
	case 5://BGIRR
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0x9);
		core->deir_en = 1;
		break;
	case 6://RIRGB
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0xa);
		core->deir_en = 1;
		break;
	case 7://BIRGR
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0xb);
		core->deir_en = 1;
		break;
	case 8://GRBIR
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0xc);
		core->deir_en = 1;
		break;
	case 9://GBRIR
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0xd);
		core->deir_en = 1;
		break;
	case 10://IRRBG
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0xe);
		core->deir_en = 1;
		break;
	case 11://IRBRG
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0xf);
		core->deir_en = 1;
		break;
	case 12://RGGIR
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0x10);
		core->deir_en = 1;
		break;
	case 13://BGGIR
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0x11);
		core->deir_en = 1;
		break;
	case 14://GRIRG
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0x12);
		core->deir_en = 1;
		break;
	case 15://GBIRG
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0x13);
		core->deir_en = 1;
		break;
	case 16://GIRRG
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0x14);
		core->deir_en = 1;
		break;
	case 17://GIRBG
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0x15);
		core->deir_en = 1;
		break;
	case 18://IRGGR
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0x16);
		core->deir_en = 1;
		break;
	case 19://IRGGB
		system_reg_write(core->priv_data, TIZIANO_BATER_TYPE, 0x17);
		core->deir_en = 1;
		break;
	default:
		ISP_ERROR("%s:%d:Not support this bayer!!!\n",__func__,__LINE__);
		break;
	}
	/* system_reg_write(core->priv_data, TIZAINO_INT_EN, 0x280f0007 | (1<<20)|(1<<21)); */
	if(core->deir_en == 1)
		core->topreg_val |= (1<<28);
	/* ae statistics position, 0 means before adr */
	if(core->ae_switch == 1)
		core->topreg_val |= (1<<26);
	system_reg_write(core->priv_data, TIZIANO_TOP_CON, core->topreg_val);

	/*TODO:*/
	sensor_init(sensor_ctrl, core->priv_data);

	/* CSC_init */
	system_reg_write(core->priv_data, CSC_ADDR_CTRL, 0x1f);
	system_reg_write(core->priv_data, CSC_ADDR_SUB, 0x0);
	/* rgb2yuv standard formula */
	//system_reg_write(core->priv_data, CSC_ADDR_PAR_0, 0x6480906);
	//system_reg_write(core->priv_data, CSC_ADDR_PAR_1, 0x1c04a497);
	//system_reg_write(core->priv_data, CSC_ADDR_PAR_2, 0x0495ddc0);
	//system_reg_write(core->priv_data, CSC_ADDR_OFF_SET, 0x8010);
	/* rgb2yuv fullrange formula bt.601-6*/
	system_reg_write(core->priv_data, CSC_ADDR_PAR_0, 0x7596532);
	system_reg_write(core->priv_data, CSC_ADDR_PAR_1, 0x20054cad);
	system_reg_write(core->priv_data, CSC_ADDR_PAR_2, 0x536b600);
	system_reg_write(core->priv_data, CSC_ADDR_OFF_SET, 0x8000);

	system_reg_write(core->priv_data, CSC_ADDR_CLIP, 0xff00ff00);

	/* SDNS_init */
	system_reg_write(core->priv_data, SDNS_ADDR_Y_FUN_EN, 0x1);
	system_reg_write(core->priv_data, SDNS_ADDR_UV_FUN_EN, 0x1);

	/* IP_init */
	/* ISP_ERROR("ByPass : %08x\n",TIZIANO_BYPASS_INIT); */
	for(i = 0;i < 16;i++){
		top_bypass = (top_bypass & (~(1 << i))) +
			(tparams.params_data.TISP_PARAM_TOP_BYPASS[0][i] << i);
	}

	system_reg_write(core->priv_data, TIZAINO_BYPASS_CON, top_bypass);
#if 1
	system_reg_write(core->priv_data, TIZAINO_BYPASS_CON, 0xffffffd7);
#endif
	system_reg_write(core->priv_data, TIZIANO_REG_CON, 1<<21);
	/* system_reg_write(core->priv_data, TIZIANO_REG_CON, 1<<21|1<<2); */

	vaddr = (unsigned int)kmalloc(4 * 1024 * 4 + 2 * 1024 * 4, GFP_KERNEL);
	if (0 == vaddr) {
		return -1;
	}
	paddr = virt_to_phys((void *)vaddr);
	system_reg_write(core->priv_data, AE_STAT_ADDR_AE_DMA_BASE_1, paddr + 4 * 1024 * 0);
	system_reg_write(core->priv_data, AE_STAT_ADDR_AE_DMA_BASE_2, paddr + 4 * 1024 * 1);
	system_reg_write(core->priv_data, AE_STAT_ADDR_AE_DMA_BASE_3, paddr + 4 * 1024 * 2);
	system_reg_write(core->priv_data, AE_STAT_ADDR_AE_DMA_BASE_4, paddr + 4 * 1024 * 3);
	system_reg_write(core->priv_data, AE_STAT_ADDR_HIST_DMA_BASE_1, paddr +4 * 1024 * 4 + 2 * 1024 * 0);
	system_reg_write(core->priv_data, AE_STAT_ADDR_HIST_DMA_BASE_2, paddr +4 * 1024 * 4 + 2 * 1024 * 1);
	system_reg_write(core->priv_data, AE_STAT_ADDR_HIST_DMA_BASE_3, paddr +4 * 1024 * 4 + 2 * 1024 * 2);
	system_reg_write(core->priv_data, AE_STAT_ADDR_HIST_DMA_BASE_4, paddr +4 * 1024 * 4 + 2 * 1024 * 3);
	/* ISP_ERROR("1=%08x,2=%08x,3=%08x,4=%08x\n",paddr+4*1024*0,paddr+4*1024*1, */
	/*        paddr+4*1024*2,paddr+4*1024*3); */

	//buf count
	system_reg_write(core->priv_data, AE_STAT_ADDR_DMA_BASE_NUM, 0x33);
	tispinfo->buf_ae_num = 4;
	tispinfo->buf_ae_vaddr = vaddr;
	tispinfo->buf_ae_paddr = paddr;
	tispinfo->buf_aehist_num = 4;
	tispinfo->buf_aehist_vaddr = vaddr + 4 * 1024 * 4;
	tispinfo->buf_aehist_paddr = paddr + 4 * 1024 * 4;

	vaddr = (unsigned int)kmalloc(4 * 1024 * 4, GFP_KERNEL);
	if (0 == vaddr) {
		return -1;
	}
	paddr = virt_to_phys((void *)vaddr);
	system_reg_write(core->priv_data, AWB_STAT_ADDR_AWB_DMA_BASE_1, paddr + 4 * 1024 * 0);
	system_reg_write(core->priv_data, AWB_STAT_ADDR_AWB_DMA_BASE_2, paddr + 4 * 1024 * 1);
	system_reg_write(core->priv_data, AWB_STAT_ADDR_AWB_DMA_BASE_3, paddr + 4 * 1024 * 2);
	system_reg_write(core->priv_data, AWB_STAT_ADDR_AWB_DMA_BASE_4, paddr + 4 * 1024 * 3);
	system_reg_write(core->priv_data, AWB_STAT_ADDR_DMA_BASE_NUM, 3);
	tispinfo->buf_awb_num = 4;
	tispinfo->buf_awb_vaddr = vaddr;
	tispinfo->buf_awb_paddr = paddr;

	vaddr = (unsigned int)kmalloc(2 * 1024 * 4, GFP_KERNEL);
	if (0 == vaddr) {
		return -1;
	}
	paddr = virt_to_phys((void *)vaddr);
	system_reg_write(core->priv_data, ADR_ADDR_BANK_ADDR_P0, paddr + 2 * 1024 * 0);
	system_reg_write(core->priv_data, ADR_ADDR_BANK_ADDR_P1, paddr + 2 * 1024 * 1);
	system_reg_write(core->priv_data, ADR_ADDR_BANK_ADDR_P2, paddr + 2 * 1024 * 2);
	system_reg_write(core->priv_data, ADR_ADDR_BANK_ADDR_P3, paddr + 2 * 1024 * 3);
	system_reg_write(core->priv_data, ADR_ADDR_BANK_NUMBER, 3);
	tispinfo->buf_adr_num = 4;
	tispinfo->buf_adr_vaddr = vaddr;
	tispinfo->buf_adr_paddr = paddr;

	vaddr = (unsigned int)kmalloc(4 * 1024 * 4, GFP_KERNEL);
	if (0 == vaddr) {
		return -1;
	}
	paddr = virt_to_phys((void *)vaddr);
	system_reg_write(core->priv_data, DEFOG_ADDR_BANK_ADDR_0, paddr + 4 * 1024 * 0);
	system_reg_write(core->priv_data, DEFOG_ADDR_BANK_ADDR_1, paddr + 4 * 1024 * 1);
	system_reg_write(core->priv_data, DEFOG_ADDR_BANK_ADDR_2, paddr + 4 * 1024 * 2);
	system_reg_write(core->priv_data, DEFOG_ADDR_BANK_ADDR_3, paddr + 4 * 1024 * 3);
	system_reg_write(core->priv_data, DEFOG_ADDR_BANK_BUFF_NUM, 3);
	tispinfo->buf_defog_num = 4;
	tispinfo->buf_defog_vaddr = vaddr;
	tispinfo->buf_defog_paddr = paddr;

	vaddr = (unsigned int)kmalloc(4 * 1024 * 4, GFP_KERNEL);
	if (0 == vaddr) {
		return -1;
	}
	paddr = virt_to_phys((void *)vaddr);
	system_reg_write(core->priv_data, AF_STAT_ADDR_AF_DMA_BASE_1, paddr + 4 * 1024 * 0);
	system_reg_write(core->priv_data, AF_STAT_ADDR_AF_DMA_BASE_2, paddr + 4 * 1024 * 1);
	system_reg_write(core->priv_data, AF_STAT_ADDR_AF_DMA_BASE_3, paddr + 4 * 1024 * 2);
	system_reg_write(core->priv_data, AF_STAT_ADDR_AF_DMA_BASE_4, paddr + 4 * 1024 * 3);
	system_reg_write(core->priv_data, AF_STAT_ADDR_DMA_BASE_NUM, 3);
	tispinfo->buf_af_num = 4;
	tispinfo->buf_af_vaddr = vaddr;
	tispinfo->buf_af_paddr = paddr;

	ae_init.min_it = p->sensor_info.min_integration_time;
	tiziano_ae_init(&core->ae, tispinfo->sensor_height,tispinfo->sensor_width,ae_init);
	tiziano_awb_init(&core->awb, tispinfo->sensor_height,tispinfo->sensor_width);
	tiziano_gamma_init(&core->gamma);
	tiziano_gib_init(&core->gib);
	tiziano_lsc_init(&core->lsc);
	tiziano_ccm_init(&core->ccm);
	tiziano_dmsc_init(&core->dmsc);
	tiziano_sharpen_init(&core->sharpen);
	tiziano_sdns_init(&core->sdns);
	tiziano_mdns_init(&core->mdns, width, height);
	tiziano_clm_init(&core->clm);
	tiziano_dpc_init(&core->dpc);
	tiziano_hldc_init(&core->hldc);
	tiziano_defog_init(&core->defog, width, height);
	tiziano_adr_init(&core->adr, width, height);
	tiziano_af_init(&core->af, height,width);
	/* isp enable */
	/* system_reg_write(core->priv_data, INPUT_CTRL_ADDR_CONTROL, 0x14); */
	system_reg_write(core->priv_data, INPUT_CTRL_ADDR_CONTROL, 0x10);//close OF_ERR_RESTART
	/* isp tirger */
	system_reg_write(core->priv_data, INPUT_CTRL_ADDR_IP_TRIG, 0x1);

	/* event list init */
	tisp_event_init(&core->tisp_event_mg);

	tisp_event_set_cb(&core->tisp_event_mg, TISP_EVENT_TYPE_AE_TGAIN_UPDATE, tisp_tgain_update, core);
	tisp_event_set_cb(&core->tisp_event_mg, TISP_EVENT_TYPE_AE_AGAIN_UPDATE, tisp_again_update, core);
	tisp_event_set_cb(&core->tisp_event_mg, TISP_EVENT_TYPE_AE_EV, tisp_ev_update, core);
	tisp_event_set_cb(&core->tisp_event_mg, TISP_EVENT_TYPE_AWB_CT, tisp_ct_update, core);
	tisp_event_set_cb(&core->tisp_event_mg, TISP_EVENT_TYPE_AE_IR, tisp_ae_ir_update, core);

	/*core_tuning init*/
	core->core_tuning.flicker_hz = 50;

	ret = tisp_param_operate_init(&core->param_operate);
	if (0 != ret) {
		ISP_ERROR("%s,%d: tisp_param_operate_init error %d\n",
		       __func__, __LINE__, ret);
	}

	return 0;
}

int tisp_core_deinit(tisp_core_t *core)
//int tisp_deinit(void)
{
	int ret = 0;
	tisp_info_t *tispinfo = &core->tispinfo;

	ret = tisp_param_operate_deinit(&core->param_operate);
	if (0 != ret) {
		ISP_ERROR("%s,%d: tisp_param_operate_deinit error %d\n",
				__func__, __LINE__, ret);
	}
	ret = tisp_event_exit(&core->tisp_event_mg);
	if (0 != ret) {
		ISP_ERROR("%s,%d: tisp_event_exit error %d\n",__func__, __LINE__, ret);
	}
	if (0 != tispinfo->buf_ae_vaddr) {
		kfree((void*)tispinfo->buf_ae_vaddr);
		tispinfo->buf_ae_vaddr = 0;
	}
	if (0 != tispinfo->buf_awb_vaddr) {
		kfree((void*)tispinfo->buf_awb_vaddr);
		tispinfo->buf_awb_vaddr = 0;
	}
	if (0 != tispinfo->buf_adr_vaddr) {
		kfree((void*)tispinfo->buf_adr_vaddr);
		tispinfo->buf_adr_vaddr = 0;
	}
	if (0 != tispinfo->buf_defog_vaddr) {
		kfree((void*)tispinfo->buf_defog_vaddr);
		tispinfo->buf_defog_vaddr = 0;
	}
	if (0 != tispinfo->buf_af_vaddr) {
		kfree((void*)tispinfo->buf_af_vaddr);
		tispinfo->buf_af_vaddr = 0;
	}

	return 0;
}

int tisp_fw_process(tisp_core_t *core)
{
	tisp_event_process(&core->tisp_event_mg);

	return 0;
}

int tisp_channel_start(tisp_core_t *core, int chx)
{
	uint32_t reg = 0;

	reg = system_reg_read(core->priv_data, MSCA_CH_EN);;
	reg |= 0x1<<chx;
	system_reg_write(core->priv_data, MSCA_CH_EN,reg);

	/* mscaler chn 0 1 2, support max resolution */
	reg = system_reg_read(core->priv_data, MSCA_DMAOUT_ARB);;
	switch (chx){
	case 0:	/*ch0 uv do not rsmp*/
		reg |= 1<<1;
		break;
	case 1:	/*ch1 uv do not rsmp*/
		reg |= 1<<2;
		break;
	case 2:	/*ch2 uv do not rsmp*/
		reg |= 1<<3;
		break;
	default:
		break;
	}
	system_reg_write(core->priv_data, MSCA_DMAOUT_ARB,reg);

	return 0;
}

static unsigned int ch_en_value;
int tisp_channel_start_restore(tisp_core_t *core)
{
	uint32_t reg = 0;
	reg = system_reg_read(core->priv_data, MSCA_CH_EN);;
	reg |= ch_en_value<<0;
	system_reg_write(core->priv_data, MSCA_CH_EN,reg);

	return 0;
}

int tisp_channel_stop(tisp_core_t *core, int chx)
{
	int ret = 0;
	unsigned int reg = 0;
	unsigned int loop = 0;
	reg = system_reg_read(core->priv_data, MSCA_CH_EN);;
	reg &= ~(0x1<<chx);
	system_reg_write(core->priv_data, MSCA_CH_EN, reg);
	do {
		reg = system_reg_read(core->priv_data, MSCA_CH_STA);;
		msleep(1);
		loop++;
		if (loop > 3000) {\
			ISP_ERROR("error(%s,%d): wait ch%d stop too long\n", __func__, __LINE__, chx);
			ret = -1;
			break;
		}
	} while (reg & (0x1<<chx));

	return 0;
}

int tisp_channel_stop_save(tisp_core_t *core)
{
	int ret = 0;
	unsigned int reg = 0;
	unsigned int loop = 0;
	reg = system_reg_read(core->priv_data, MSCA_CH_EN);;
	ch_en_value = reg & 0x7;;
	reg &= ~(0x7<<0);
	system_reg_write(core->priv_data, MSCA_CH_EN, reg);

	do {
		reg = system_reg_read(core->priv_data, MSCA_CH_STA);;
		msleep(1);
		loop++;
		if (loop > 3000) {\
			ISP_ERROR("error(%s,%d): wait all channel stop too long\n", __func__, __LINE__);
			ret = -1;
			break;
		}
	} while (reg & (0x7<<0));

	return 0;
}

int tisp_channel_attr_set_crop_scaler(tisp_core_t *core, int chx ,tisp_channel_attr_t * cattr)
{
	uint32_t src_w;
	uint32_t src_h;
	uint32_t dst_w;
	uint32_t dst_h;
	uint16_t step_w;
	uint16_t step_h;
	uint32_t step;
	tisp_info_t *tispinfo = &core->tispinfo;

	system_reg_write(core->priv_data, CHx_RSZ_OSIZE(chx), (cattr->scaler_width<<16)|cattr->scaler_height);
	src_w = tispinfo->sensor_width;
	src_h = tispinfo->sensor_height;
	dst_w = cattr->scaler_width;
	dst_h = cattr->scaler_height;
	step_w = src_w*512/dst_w;
	step_h = src_h*512/dst_h;
	step = (step_w << 16)|step_h;

	system_reg_write(core->priv_data, CHx_RSZ_STEP(chx), step);
	system_reg_write(core->priv_data, CHx_CROP_OSIZE(chx), (cattr->crop_width<<16)|cattr->crop_height);
	system_reg_write(core->priv_data, CHx_CROP_OPOS(chx), (cattr->crop_x << 16)|cattr->crop_y);

	system_reg_write(core->priv_data, MSCA_DMAOUT_ARB,0xe);

	return 0;
}

int tisp_channel_attr_set(tisp_core_t *core, int chx ,tisp_channel_attr_t * cattr)
{
	uint32_t src_w;
	uint32_t src_h;
	uint32_t dst_w;
	uint32_t dst_h;
	uint16_t step_w;
	uint16_t step_h;
	uint32_t step;
	tisp_info_t *tispinfo = &core->tispinfo;

	system_reg_write(core->priv_data, CHx_RSZ_OSIZE(chx), (cattr->scaler_width<<16)|cattr->scaler_height);
	src_w = tispinfo->sensor_width;
	src_h = tispinfo->sensor_height;
	dst_w = cattr->scaler_width;
	dst_h = cattr->scaler_height;
	step_w = src_w*512/dst_w;
	step_h = src_h*512/dst_h;
	step = (step_w << 16)|step_h;
	/* ISP_ERROR("src_w = %d,src_h = %d,dst_w = %d,dst_h = %d \n",src_w,src_h,dst_w,dst_h); */
	system_reg_write(core->priv_data, CHx_RSZ_STEP(chx), step);
	system_reg_write(core->priv_data, CHx_CROP_OSIZE(chx), (cattr->crop_width<<16)|cattr->crop_height);
	system_reg_write(core->priv_data, CHx_CROP_OPOS(chx), (cattr->crop_x << 16)|cattr->crop_y);

	system_reg_write(core->priv_data, CHx_DMAOUT_Y_STRI(chx), cattr->crop_width);
	system_reg_write(core->priv_data, CHx_DMAOUT_UV_STRI(chx), cattr->crop_width);

	return 0;
}

static const unsigned int crc_table[8] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
	0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
};

static unsigned int crc32(const unsigned int *p, unsigned int len)
{
	int i = 0;
	unsigned int crc = crc_table[0];
	for(i = 0; i < len; i++){
		crc ^= *p++;
		crc = crc ^ crc_table[crc & 0x7];
	}

	return crc;
}

int tiziano_load_parameters(tisp_core_t *core)
{
	unsigned int ret = 0;
	int index = 0;
	struct file *file = NULL;
	struct inode *inode = NULL;
	mm_segment_t old_fs;
	loff_t fsize;
	loff_t *pos;
	tisp_init_param_t *sensor_info 		= &core->sensor_info;

	char file_name[64];
	char *cursor = NULL;
	TXispPrivParamHeader *header = NULL;

	TISPParamManage *m = kmalloc(sizeof(*m), GFP_KERNEL);
	if(!m){
		ISP_ERROR("Failed to kmalloc TISPParamManage\n");
		return -1;
	}
	memset(m, 0, sizeof(*m));
	snprintf(m->version, sizeof(m->version), "%s", TISP_VERSION_ID);

	snprintf(m->headers[index].flag, TISP_PRIV_PARAM_FLAG_SIZE, "header%d", index);
	/* open file */
	snprintf(file_name, sizeof(file_name), "/etc/sensor/%s-t21.bin", sensor_info->sensor);
	file = filp_open(file_name, O_RDONLY, 0);
	if (file < 0 || IS_ERR(file)) {
		ISP_ERROR("ISP: open %s file for isp calibrate read failed\n", file_name);
		ret = -1;
		goto failed_open_file;
	}

	/* read file */
	//inode = file->f_dentry->d_inode;
	inode = file->f_inode;
	fsize = inode->i_size;
	old_fs = get_fs();
	set_fs(KERNEL_DS);
	pos = &(file->f_pos);

	if(m->data == NULL){
		m->data = kmalloc(fsize, GFP_KERNEL);
		if(m->data == NULL){
			ISP_ERROR("%s[%d]: Failed to alloc %lld KB buffer!\n",__func__,__LINE__, fsize >> 10);
			ret = -1;
			goto failed_malloc_data;
		}
		m->data_size = fsize;
	}
	if(m->data_size < fsize){
		ISP_ERROR("The size of file has been modify!\n");
		ret = -1;
		goto failed_check_total_size;
	}

	vfs_read(file, m->data, fsize, pos);
	filp_close(file, NULL);
	set_fs(old_fs);

	cursor = m->data;
	if(strncmp(m->version, cursor, TISP_VERSION_SIZE)){
		ret = -1;
		ISP_ERROR("####################################################################\n");
		ISP_ERROR("#### The version of bin doesn't match with driver! ####\n");
		ISP_ERROR("#### The version of bin is %s, the driver is %s ####\n", m->version, cursor);
		ISP_ERROR("####################################################################\n");
		goto failed_check_version;
	}
	/* ISP_ERROR("@@@@@@@@@@m-version is %s\n",m->version); */
	cursor += TISP_VERSION_SIZE;
	header = &m->headers[TISP_PRIV_PARAM_BASE_INDEX];
	/* ISP_ERROR("########header flag is %s\n",header->flag); */
	if(strncmp((char *)header, cursor, TISP_PRIV_PARAM_FLAG_SIZE)){
		ret = -1;
		ISP_ERROR("####################################################################\n");
		ISP_ERROR("#### The first flag of bin doesn't match with driver! ####\n");
		ISP_ERROR("####################################################################\n");
		goto failed_check_header0;
	}
	header->size = ((TXispPrivParamHeader *)cursor)->size;
	header->crc = ((TXispPrivParamHeader *)cursor)->crc;
	cursor += sizeof(TXispPrivParamHeader);
	if(header->crc != crc32((unsigned int*)cursor, header->size / 4)){
		ISP_ERROR("%s[%d]: Failed to CRC sensor setting!\n",__func__,__LINE__);
		ret = -1;
		goto failed_crc_header0;
	}

	m->base_buf = cursor;
	memcpy((void*)(&tparams_day.params_data),(void *)cursor,sizeof(tisp_params_data_t));
	cursor += sizeof(tisp_params_data_t);
	memcpy((void*)(&tparams_night.params_data),(void *)cursor,sizeof(tisp_params_data_t));

	kfree(m->data);
	kfree(m);

	return 0;

failed_crc_header0:
failed_check_header0:
failed_check_total_size:
failed_malloc_data:
	filp_close(file, NULL);
	set_fs(old_fs);
failed_check_version:
	kfree(m->data);
	kfree(m);
failed_open_file:
	return -1;
}
