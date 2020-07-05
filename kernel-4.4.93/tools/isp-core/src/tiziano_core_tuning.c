#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include "../inc/system_sensor_drv.h"
#include "tiziano_map.h"
#include "../inc/tiziano_isp.h"
#include "tiziano_core.h"
#include "../inc/tiziano_sys.h"
#include "tiziano_ae.h"
#include "tiziano_af.h"
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
#include "tiziano_params_operate.h"
#include "tiziano_params.h"
#include "fix_point_calc.h"
#include "../inc/tiziano_core_tuning.h"

int32_t tisp_day_or_night_s_ctrl(tisp_core_tuning_t *core_tuning, TISP_MODE_DN_E dn)
{
	unsigned int top_bypass;
	tisp_core_t *core = core_tuning->core;
	int i = 0;

	if(dn == TISP_MODE_DAY_MODE){
		memcpy(&tparams, &tparams_day, sizeof(tparams));
		core_tuning->day_night = 0;
		system_reg_write(((tisp_core_t *)core_tuning->core)->priv_data, TIZIANO_BASE + 0x1730, 0xff00ff00);
	} else if (dn == TISP_MODE_NIGHT_MODE) {
		memcpy(&tparams, &tparams_night, sizeof(tparams));
		core_tuning->day_night = 1;
		system_reg_write(((tisp_core_t *)core_tuning->core)->priv_data, TIZIANO_BASE + 0x1730, 0xff008080);
	} else {
		ISP_ERROR("%s:%d:can not support this mode!!!",__func__,__LINE__);
	}
	top_bypass = system_reg_read(((tisp_core_t *)core_tuning->core)->priv_data, TIZAINO_BYPASS_CON);

	for(i = 0;i < 16;i++){
		top_bypass = (top_bypass & (~(1 << i))) +
			(tparams.params_data.TISP_PARAM_TOP_BYPASS[0][i] << i);
	}
	system_reg_write(((tisp_core_t *)core_tuning->core)->priv_data, TIZAINO_BYPASS_CON, top_bypass);

	tiziano_dpc_dn_params_refresh(&core->dpc);
	tiziano_gib_dn_params_refresh(&core->gib);
	tiziano_lsc_dn_params_refresh(&core->lsc);
	tiziano_ae_dn_params_refresh(&core->ae);
	tiziano_awb_dn_params_refresh(&core->awb);
	tiziano_adr_dn_params_refresh(&core->adr);
	tiziano_dmsc_dn_params_refresh(&core->dmsc);
	tiziano_gamma_dn_params_refresh(&core->gamma);
	tiziano_ccm_dn_params_refresh(&core->ccm);
	tiziano_defog_dn_params_refresh(&core->defog);
	tiziano_clm_dn_params_refresh(&core->clm);
	tiziano_sharpen_dn_params_refresh(&core->sharpen);
	tiziano_mdns_dn_params_refresh(&core->mdns);
	tiziano_sdns_dn_params_refresh(&core->sdns);
	tiziano_af_dn_params_refresh(&core->af);

	return 0;
}


TISP_MODE_DN_E tisp_day_or_night_g_ctrl(tisp_core_tuning_t *core_tuning)
{
	if(core_tuning->day_night == 0)
		return TISP_MODE_DAY_MODE;
	else if(core_tuning->day_night == 1)
		return TISP_MODE_NIGHT_MODE;
	else
		return -1;
}

void tisp_mirror_enable(tisp_core_tuning_t *core_tuning, int en)
{
	unsigned int mirror = system_reg_read(((tisp_core_t *)core_tuning->core)->priv_data, TIZIANO_BASE + 0x2318);
	system_reg_write(((tisp_core_t *)core_tuning->core)->priv_data, TIZIANO_BASE + 0x2318, en ? (mirror | 0x380):(mirror & 0xfffffc7f));
}

void tisp_flip_enable(tisp_core_tuning_t *core_tuning, int en)
{
	unsigned int flip = system_reg_read(((tisp_core_t *)core_tuning->core)->priv_data, TIZIANO_BASE + 0x2318);
	system_reg_write(((tisp_core_t *)core_tuning->core)->priv_data, TIZIANO_BASE + 0x2318, en ? (flip | 0x70):(flip & 0xffffff8f));
}

int tisp_set_fps(tisp_core_tuning_t *core_tuning, int fps)
{
	int ret = 0;
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;

	ret = core->sensor_ctrl_ops->fps_control(&core->sensor_ctrl, (unsigned char)((fps >> 16) / (fps & 0xffff)), &core->sensor_ctrl.param);
	if(ret < 0) {
		ISP_ERROR("fps get wrong!\n");
		return -1;
	}

	core->ae.tisp_ae_ctrls.tisp_ae_max_sensor_integration_time = core->sensor_ctrl.param.integration_time_max;
	core->sensor_info.sensor_info.fps = ret;
	core->sensor_info.sensor_info.min_integration_time = core->sensor_ctrl.param.integration_time_min;
	core->sensor_info.sensor_info.min_integration_time_native = core->sensor_ctrl.param.integration_time_min;
	core->sensor_info.sensor_info.max_integration_time_native = core->sensor_ctrl.param.integration_time_max;
	core->sensor_info.sensor_info.integration_time_limit = core->sensor_ctrl.param.integration_time_limit;
	core->sensor_info.sensor_info.max_integration_time = core->sensor_ctrl.param.integration_time_max;
	core->sensor_info.sensor_info.total_width = core->sensor_ctrl.param.total.width;
	core->sensor_info.sensor_info.total_height = core->sensor_ctrl.param.total.height;
/* update antiflicker params */
	if(core_tuning->flicker_hz != 0){
		tiziano_deflicker_expt_tune(&core->ae, core_tuning->flicker_hz, core->sensor_info.sensor_info.fps,
					    core->sensor_info.sensor_info.total_height,
					    core->sensor_info.sensor_info.total_width);
		/* ISP_ERROR("####%d,%d,%d,%d\n",flicker_hz, sensor_info.sensor_info.fps,
		   sensor_info.sensor_info.total_height,
		   sensor_info.sensor_info.total_width); */
	}
	/* ISP_ERROR("$$$%d\n",flicker_hz); */

	return 0;
}

void tisp_set_brightness(tisp_core_tuning_t *core_tuning, unsigned char brightness)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
#if 1
	tiziano_ae_compensation_set(&core->ae, brightness); // use ae compensation to change brightness, ev will be effected
#else
{
	int i = 0;
	int target_day[20];
	int target_night[20];
	int size = sizeof(tparams.params_data.TISP_PARAM_AE_AT_LIST);

	size *= 2; //AT_LIST/EV_LIST
	memcpy(target_day, custom_eff.brightness_ori_day, size);
	memcpy(target_night, custom_eff.brightness_ori_night, size);

	for(i = 0; i < size / sizeof(int); i++){
		target_day[i] = target_day[i] * brightness / 128;
		target_night[i] = target_night[i] * brightness / 128;
		if(target_day[i] <= 0)
			target_day[i] = 1;
		if(target_night[i] <= 0)
			target_night[i] = 1;
	}

	size /= 2;
	if (0 == day_night){
		tisp_ae_param_array_set(TISP_PARAM_AE_AT_LIST, target_day, &size);
		tisp_ae_param_array_set(TISP_PARAM_AE_EV_LIST, &target_day[10], &size);
		memcpy(tparams.params_data.TISP_PARAM_AE_AT_LIST, target_day,
		       sizeof(tparams.params_data.TISP_PARAM_AE_AT_LIST));
		memcpy(tparams.params_data.TISP_PARAM_AE_EV_LIST, &target_day[10],
		       sizeof(tparams.params_data.TISP_PARAM_AE_EV_LIST));
	} else if(1 == day_night){
		tisp_ae_param_array_set(TISP_PARAM_AE_AT_LIST, target_night, &size);
		tisp_ae_param_array_set(TISP_PARAM_AE_EV_LIST, &target_night[10], &size);
		memcpy(tparams.params_data.TISP_PARAM_AE_AT_LIST, target_night,
		       sizeof(tparams.params_data.TISP_PARAM_AE_AT_LIST));
		memcpy(tparams.params_data.TISP_PARAM_AE_EV_LIST, &target_night[10],
		       sizeof(tparams.params_data.TISP_PARAM_AE_EV_LIST));

	} else {
		ISP_ERROR("%s:%d::Can't support this day or night mode!!!\n",__func__,__LINE__);
	}
	memcpy(tparams_day.params_data.TISP_PARAM_AE_AT_LIST, target_day,
	       sizeof(tparams.params_data.TISP_PARAM_AE_AT_LIST));
	memcpy(tparams.params_data.TISP_PARAM_AE_EV_LIST, &target_day[10],
	       sizeof(tparams.params_data.TISP_PARAM_AE_EV_LIST));
	memcpy(tparams_night.params_data.TISP_PARAM_AE_AT_LIST, target_night,
	       sizeof(tparams.params_data.TISP_PARAM_AE_AT_LIST));
	memcpy(tparams.params_data.TISP_PARAM_AE_EV_LIST, &target_night[10],
	       sizeof(tparams.params_data.TISP_PARAM_AE_EV_LIST));
}
#endif

	core->custom_eff.brightness = brightness;
	/* ISP_ERROR("#####%d,%d,%d,%d\n",custom_eff.brightness,custom_eff.saturation,custom_eff.contrast,custom_eff.sharpness); */
}

void tisp_set_sharpness(tisp_core_tuning_t *core_tuning, unsigned char sharpness)
{
	tisp_core_t *core = core_tuning->core;
	int i = 0;
	int target_day[36];
	int target_night[36];
	int size = sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY);
	size *= 4;//DW/DB/UDW/UDB

	memcpy(target_day, core->custom_eff.sharpen_ori_day, size);
	memcpy(target_night, core->custom_eff.sharpen_ori_night, size);

	for(i = 0; i < size / sizeof(int); i++){
		target_day[i] = target_day[i] * sharpness / 128;
		target_night[i] = target_night[i] * sharpness / 128;
		if(target_day[i] <= 0)
			target_day[i] = 0;
		if(target_night[i] <= 0)
			target_night[i] = 0;
		if(target_day[i] >= 255)
			target_day[i] = 255;
		if(target_night[i] >= 255)
			target_night[i] = 255;
	}

	size /= 4;
	if (0 == core_tuning->day_night){
		tisp_dmsc_param_array_set(&core->dmsc, TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY, target_day, &size);
		tisp_dmsc_param_array_set(&core->dmsc, TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY, &target_day[9], &size);
		tisp_dmsc_param_array_set(&core->dmsc, TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY, &target_day[18],&size);
		tisp_dmsc_param_array_set(&core->dmsc, TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY, &target_day[27], &size);
		memcpy(tparams.params_data.TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY, target_day,
		       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY));
		memcpy(tparams.params_data.TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY, &target_day[9],
		       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY));
		memcpy(tparams.params_data.TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY, &target_day[18],
		       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY));
		memcpy(tparams.params_data.TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY, &target_day[27],
		       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY));
	} else if(1 == core_tuning->day_night){
		tisp_dmsc_param_array_set(&core->dmsc, TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY, target_night, &size);
		tisp_dmsc_param_array_set(&core->dmsc, TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY, &target_night[9], &size);
		tisp_dmsc_param_array_set(&core->dmsc, TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY, &target_night[18],&size);
		tisp_dmsc_param_array_set(&core->dmsc, TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY, &target_night[27], &size);
		memcpy(tparams.params_data.TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY, target_night,
		       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY));
		memcpy(tparams.params_data.TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY, &target_night[9],
		       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY));
		memcpy(tparams.params_data.TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY, &target_night[18],
		       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY));
		memcpy(tparams.params_data.TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY, &target_night[27],
		       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY));

	} else {
		ISP_ERROR("%s:%d::Can't support this day or night mode!!!\n",__func__,__LINE__);
	}

	memcpy(tparams_day.params_data.TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY, target_day,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY));
	memcpy(tparams_day.params_data.TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY, &target_day[9],
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY));
	memcpy(tparams_day.params_data.TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY, &target_day[18],
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY));
	memcpy(tparams_day.params_data.TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY, &target_day[27],
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY));
	memcpy(tparams_night.params_data.TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY, target_night,
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_W_STREN_ARRAY));
	memcpy(tparams_night.params_data.TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY, &target_night[9],
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_D_B_STREN_ARRAY));
	memcpy(tparams_night.params_data.TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY, &target_night[18],
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_W_STREN_ARRAY));
	memcpy(tparams_night.params_data.TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY, &target_night[27],
	       sizeof(tparams.params_data.TISP_PARAM_DMSC_SP_UD_B_STREN_ARRAY));

	core->custom_eff.sharpness = sharpness;
	/* ISP_ERROR("#####%d,%d,%d,%d\n",custom_eff.sharpness,custom_eff.saturation,custom_eff.contrast,custom_eff.brightness); */
}

void tisp_set_saturation(tisp_core_tuning_t *core_tuning, unsigned char saturation)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	int i = 0;
	int target_day[9];
	int size = sizeof(tparams.params_data.TISP_PARAM_CCM_SAT_LIST);

	memcpy(target_day, core->custom_eff.saturation_ori_day, size);
	for(i = 0;i < size/sizeof(int);i++){
		target_day[i] = target_day[i] * saturation / 128;
		if(target_day[i] <= 0)
			target_day[i] = 0;
	}

	if(0 == core_tuning->day_night){
		tisp_ccm_param_array_set(&core->ccm, TISP_PARAM_CCM_SAT_LIST, target_day, &size);
		memcpy(tparams.params_data.TISP_PARAM_CCM_SAT_LIST, target_day,
		       sizeof(tparams.params_data.TISP_PARAM_CCM_SAT_LIST));
		memcpy(tparams_day.params_data.TISP_PARAM_CCM_SAT_LIST, target_day,
		       sizeof(tparams.params_data.TISP_PARAM_CCM_SAT_LIST));
		core->custom_eff.saturation = saturation;
	} else if(1 == core_tuning->day_night)
		core->custom_eff.saturation = 0;
}

void tisp_set_contrast(tisp_core_tuning_t *core_tuning, unsigned char contrast)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	unsigned int con_val;

	if (contrast >= 128){
		con_val = system_reg_read(((tisp_core_t *)core_tuning->core)->priv_data, CSC_BASE + 0x20);
		system_reg_write(((tisp_core_t *)core_tuning->core)->priv_data, CSC_BASE + 0x20, con_val & 0xffffff00);

		con_val = system_reg_read(((tisp_core_t *)core_tuning->core)->priv_data, CSC_BASE + 0x04);
		con_val = (con_val & 0xff000000) + (contrast - 128) + ((contrast - 128) << 8) + ((contrast - 128) << 16);
		system_reg_write(((tisp_core_t *)core_tuning->core)->priv_data, CSC_BASE + 0x04,con_val);
	} else if (contrast < 128){
		con_val = system_reg_read(((tisp_core_t *)core_tuning->core)->priv_data, CSC_BASE + 0x04);
		system_reg_write(((tisp_core_t *)core_tuning->core)->priv_data, CSC_BASE + 0x04, con_val & 0xff000000);

		con_val = system_reg_read(((tisp_core_t *)core_tuning->core)->priv_data, CSC_BASE + 0x20);
		con_val = (con_val & 0xffffff00) + 0x10 + (0x50 * (128 -  contrast) / 128);
		system_reg_write(((tisp_core_t *)core_tuning->core)->priv_data, CSC_BASE + 0x20, con_val);
	}

	core->custom_eff.contrast = contrast;
}

unsigned char tisp_get_brightness(tisp_core_tuning_t *core_tuning)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	return core->custom_eff.brightness;
}

unsigned char tisp_get_sharpness(tisp_core_tuning_t *core_tuning)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	return core->custom_eff.sharpness;
}

unsigned char tisp_get_saturation(tisp_core_tuning_t *core_tuning)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	return 	core->custom_eff.saturation;
}

unsigned char tisp_get_contrast(tisp_core_tuning_t *core_tuning)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	return 	core->custom_eff.contrast;
}

void tisp_top_sel(tisp_core_tuning_t *core_tuning, int sel)
{
	unsigned int top_bypass = system_reg_read(((tisp_core_t *)core_tuning->core)->priv_data, TIZAINO_BYPASS_CON);

	top_bypass |= 0x80000000;
	system_reg_write(((tisp_core_t *)core_tuning->core)->priv_data, TIZAINO_BYPASS_CON, top_bypass);
}

unsigned int tisp_top_read(tisp_core_tuning_t *core_tuning)
{
	return system_reg_read(((tisp_core_t *)core_tuning->core)->priv_data, TIZAINO_BYPASS_CON);
}

int tisp_g_ncuinfo(tisp_core_tuning_t *core_tuning, tisp_ncu_info_t *ncuinfo)
{
	if (ncuinfo == NULL){
		ISP_ERROR("The ncu info addr is NULL!!!\n");
		return -1;
	}

	ncuinfo->width = system_reg_read(((tisp_core_t *)core_tuning->core)->priv_data, TIZIANO_FM_SIZE) >> 16;
	ncuinfo->height = system_reg_read(((tisp_core_t *)core_tuning->core)->priv_data, TIZIANO_FM_SIZE) & 0xffff;
	ncuinfo->sta_y_block_size = system_reg_read(((tisp_core_t *)core_tuning->core)->priv_data, MDNS_ADDR_STA_BLK_SIZE) & 0xffff;
	ncuinfo->sta_y_stride = system_reg_read(((tisp_core_t *)core_tuning->core)->priv_data, MDNS_ADDR_STA_STRIDE);
	ncuinfo->sta_y_buf_size = system_reg_read(((tisp_core_t *)core_tuning->core)->priv_data, MDNS_ADDR_STA_FRM_SIZE);

	return 0;
}

int tisp_s_antiflick(tisp_core_tuning_t *core_tuning, int hz)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	int deflickpara[3];
	int deflicker_lt[6];
	int size = sizeof(tparams.params_data.TISP_PARAM_AE_DEFLICKER_PARA);
	int size_lt = sizeof(tparams.params_data.TISP_PARAM_AE_FLICKER_T);
	if(hz != 0)
		core_tuning->flicker_hz = hz;
	memcpy(deflickpara, tparams.params_data.TISP_PARAM_AE_DEFLICKER_PARA, size);
	memcpy(deflicker_lt, tparams.params_data.TISP_PARAM_AE_FLICKER_T, size_lt);

	switch(hz){
	case 0:
		deflickpara[0] = 0;
		break;
	case 50:
		deflickpara[0] = 1;
		break;
	case 60:
		deflickpara[0] = 1;
		break;
	default:
		ISP_ERROR("%s,%d,Cant support this hz(%d)!!!\n",__func__,__LINE__,hz);
		return -1;
	}

	tiziano_deflicker_expt_tune(&core->ae, core_tuning->flicker_hz, core->sensor_info.sensor_info.fps, core->sensor_info.sensor_info.total_height,
				    core->sensor_info.sensor_info.total_width);

	deflicker_lt[0] = core_tuning->flicker_hz;
	deflicker_lt[1] = core->sensor_info.sensor_info.fps;
	deflicker_lt[2] = core->sensor_info.sensor_info.total_height;
	deflicker_lt[3] = core->sensor_info.sensor_info.total_width;
	memcpy(tparams.params_data.TISP_PARAM_AE_DEFLICKER_PARA, deflickpara, size);
	tisp_ae_param_array_set(&core->ae, TISP_PARAM_AE_DEFLICKER_PARA, deflickpara, &size);
	memcpy(tparams.params_data.TISP_PARAM_AE_FLICKER_T, deflicker_lt, size_lt);
	tisp_ae_param_array_set(&core->ae, TISP_PARAM_AE_FLICKER_T, deflicker_lt, &size_lt);

	memcpy(tparams_day.params_data.TISP_PARAM_AE_DEFLICKER_PARA, deflickpara, size);
	memcpy(tparams_night.params_data.TISP_PARAM_AE_DEFLICKER_PARA, deflickpara, size);
	memcpy(tparams_day.params_data.TISP_PARAM_AE_FLICKER_T, deflicker_lt, size_lt);
	memcpy(tparams_night.params_data.TISP_PARAM_AE_FLICKER_T, deflicker_lt, size_lt);

	return 0;
}

int tisp_s_Hilightdepress(tisp_core_tuning_t *core_tuning, unsigned int strength)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	int scencepara[11];
	int size = sizeof(tparams.params_data.TISP_PARAM_AE_SCENE_PARE);

	memcpy(scencepara, tparams.params_data.TISP_PARAM_AE_SCENE_PARE, size);

	scencepara[0] = 1;
	scencepara[6] = strength + 1;

	memcpy(tparams.params_data.TISP_PARAM_AE_SCENE_PARE, scencepara, size);
	tisp_ae_param_array_set(&core->ae, TISP_PARAM_AE_SCENE_PARE, scencepara, &size);

	/* memcpy(tparams_day.params_data.TISP_PARAM_AE_SCENE_PARE, scencepara, size); */
	/* memcpy(tparams_night.params_data.TISP_PARAM_AE_SCENE_PARE, scencepara, size); */

	return 0;
}

int tisp_g_Hilightdepress(tisp_core_tuning_t *core_tuning, unsigned int *strength)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	int scencepara[11];
	int size = sizeof(tparams.params_data.TISP_PARAM_AE_SCENE_PARE);
	int retsize = 0;

	/* memcpy(scencepara, tparams.params_data.TISP_PARAM_AE_SCENE_PARE, size); */
	tisp_ae_param_array_get(&core->ae, TISP_PARAM_AE_SCENE_PARE,scencepara, &retsize);
	if(retsize != size) {
		ISP_ERROR("%s,%d:::Get Hilight depress failed!!!\n",__func__,__LINE__);
		return -1;
	}
	if(scencepara[0] == 0)
		*strength = 0;
	else if(scencepara[6] == 1)
		*strength = 0;
	else
		*strength = (scencepara[6] -  1);

	return 0;
}

int tisp_s_Gamma(tisp_core_tuning_t *core_tuning, tisp_gamma_lut_t *gammas)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;

	int size = sizeof(tparams.params_data.TISP_PARAM_GAMMA_RGB_LUT_SADDR);

	memcpy(tparams.params_data.TISP_PARAM_GAMMA_RGB_LUT_SADDR, gammas, size);
	tisp_gamma_param_array_set(&core->gamma, TISP_PARAM_GAMMA_RGB_LUT_SADDR, gammas, &size);

	memcpy(tparams_day.params_data.TISP_PARAM_GAMMA_RGB_LUT_SADDR, gammas, size);
	memcpy(tparams_night.params_data.TISP_PARAM_GAMMA_RGB_LUT_SADDR, gammas, size);

	return 0;
}

int tisp_g_Gamma(tisp_core_tuning_t *core_tuning, tisp_gamma_lut_t *gammag)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	int size = sizeof(tparams.params_data.TISP_PARAM_GAMMA_RGB_LUT_SADDR);
	int retsize = 0;

	/* memcpy(gammag->gamma, tparams.params_data.TISP_PARAM_GAMMA_RGB_LUT_SADDR, size); */
	tisp_gamma_param_array_get(&core->gamma, TISP_PARAM_GAMMA_RGB_LUT_SADDR,gammag->gamma, &retsize);
	if(retsize == size) {
		return 0;
	} else {
		ISP_ERROR("%s,%d:::Get Gamma failed!!!\n",__func__,__LINE__);
		return -1;
	}
}

int tisp_s_aeroi_weight(tisp_core_tuning_t *core_tuning, tisp_3a_weight_t * ae_weight)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	tisp_3a_weight_t ae_roui;
	int i = 0;
	int size = sizeof(tparams.params_data.TISP_PARAM_AE_SCENE_ROI_WEIGHT);

	for(i = 0; i < 225;i++){
		ae_roui.weight[i] = 8 - ae_weight->weight[i];
	}

	memcpy(tparams.params_data.TISP_PARAM_AE_SCENE_ROI_WEIGHT, ae_weight, size);
	memcpy(tparams.params_data.TISP_PARAM_AE_SCENE_ROUI_WEIGHT, &ae_roui, size);
	tisp_ae_param_array_set(&core->ae, TISP_PARAM_AE_SCENE_ROI_WEIGHT, ae_weight, &size);
	tisp_ae_param_array_set(&core->ae, TISP_PARAM_AE_SCENE_ROUI_WEIGHT, &ae_roui, &size);

	return 0;
}

int tisp_g_aeroi_weight(tisp_core_tuning_t *core_tuning, tisp_3a_weight_t * ae_weight)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	int size = sizeof(tparams.params_data.TISP_PARAM_AE_SCENE_ROI_WEIGHT);
	int retsize = 0;

	tisp_ae_param_array_get(&core->ae, TISP_PARAM_AE_SCENE_ROI_WEIGHT,ae_weight, &retsize);

	if(retsize == size) {
		return 0;
	} else {
		ISP_ERROR("%s,%d:::Get ae_roi failed!!!\n",__func__,__LINE__);
		return -1;
	}
}

int tisp_s_af_weight(tisp_core_tuning_t *core_tuning, tisp_3a_weight_t * af_weight)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	int size = sizeof(tparams.params_data.TISP_PARAM_AF_WEIGHT);

	memcpy(tparams.params_data.TISP_PARAM_AF_WEIGHT, af_weight, size);
	tisp_af_param_array_set(&core->af, TISP_PARAM_AF_WEIGHT, af_weight, &size);

	return 0;
}

int tisp_g_af_weight(tisp_core_tuning_t *core_tuning, tisp_3a_weight_t * af_weight)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	int size = sizeof(tparams.params_data.TISP_PARAM_AF_WEIGHT);
	int retsize = 0;

	tisp_af_param_array_get(&core->af, TISP_PARAM_AF_WEIGHT,af_weight, &retsize);

	if(retsize == size) {
		return 0;
	} else {
		ISP_ERROR("%s,%d:::Get af_weight failed!!!\n",__func__,__LINE__);
		return -1;
	}
}

int tisp_g_ev_attr(tisp_core_tuning_t *core_tuning, tisp_ev_attr_t *ev_attr)
{
	unsigned int total_gain = 0;
	tisp_core_t *core = core_tuning->core;
	tisp_ae_t ae = core->ae;

	ev_attr->integration_time = ae.tisp_ae_ctrls.tisp_ae_sensor_integration_time;
	ev_attr->ev = ae.tisp_ae_ctrls.tisp_ae_ev >> 10;
	ev_attr->ev_log2 = tisp_log2_fixed_to_fixed(ae.tisp_ae_ctrls.tisp_ae_ev,10,16);
	ev_attr->expr_us = 1000000 * ev_attr->integration_time *
		(core->sensor_info.sensor_info.fps & 0xffff) /
		(core->sensor_info.sensor_info.fps >> 16) /
		core->sensor_info.sensor_info.total_height;
	ev_attr->again = tisp_log2_fixed_to_fixed(ae.tisp_ae_ctrls.tisp_ae_sensor_again,10,5);
	ev_attr->dgain = tisp_log2_fixed_to_fixed(ae.tisp_ae_ctrls.tisp_ae_isp_dgian,10,5);
	ev_attr->gain_log2 = ae.tisp_ae_ctrls.tisp_ae_tgain_db >> 16;
	total_gain = fix_point_mult2_32(10, ae.tisp_ae_ctrls.tisp_ae_sensor_again, ae.tisp_ae_ctrls.tisp_ae_isp_dgian);
	ev_attr->total_gain = total_gain >> (10 - 8);//from 22.10 to 24.8
	/* +4 to complement the loss from accuracy conversion, so the get value is same as set value */
	ev_attr->max_again = tisp_log2_fixed_to_fixed(ae.tisp_ae_ctrls.tisp_ae_max_sensor_again+4,10,5);
	ev_attr->max_isp_dgain = tisp_log2_fixed_to_fixed(ae.tisp_ae_ctrls.tisp_ae_max_isp_dgain+4,10,5);
	ev_attr->sensor_dgain = tisp_log2_fixed_to_fixed(ae.tisp_ae_ctrls.tisp_ae_sensor_dgain,10,5);
	ev_attr->max_sensor_dgain = tisp_log2_fixed_to_fixed(ae.tisp_ae_ctrls.tisp_ae_max_sensor_dgain,10,5);

	return 0;
}

int tisp_g_wb_attr(tisp_core_tuning_t *core_tuning, tisp_wb_attr_t *wb_attr)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;

	tisp_g_wb_mode(&core->awb, wb_attr);

	return 0;
}

int tisp_s_wb_attr(tisp_core_tuning_t *core_tuning, tisp_wb_attr_t wb_attr)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;

	tisp_s_wb_mode(&core->awb, wb_attr);

	return 0;
}

int tisp_g_ae_hist(tisp_core_tuning_t *core_tuning, tisp_ae_sta_t *ae_hist)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;

	tisp_ae_get_hist_custome(&core->ae, ae_hist);

	return 0;
}

int tisp_s_ae_hist(tisp_core_tuning_t *core_tuning, tisp_ae_sta_t ae_hist)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;

	tisp_ae_set_hist_custome(&core->ae, ae_hist);

	return 0;
}

int tisp_s_3dns_ratio(tisp_core_tuning_t *core_tuning, unsigned int ratio)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	int i = 0;
	int dysad_thres_day[9];
	int dysta_thres_day[9];
	int dypbt_thres_day[9];
	int dywei_max_day[9];
	int dywei_min_day[9];
	int dysad_thres_night[9];
	int dysta_thres_night[9];
	int dypbt_thres_night[9];
	int dywei_max_night[9];
	int dywei_min_night[9];
	int size = sizeof(tparams.params_data.TISP_PARAM_MDSN_Y_SAD_THRES_ARRAY);

	for(i = 0; i < 9; i++){
		if(ratio <= 128){
			dysad_thres_day[i] = ratio * core->mdns_ratio.dysad_thres_def_day[i] / 128;
			dysta_thres_day[i] = ratio * core->mdns_ratio.dysta_thres_def_day[i] / 128;
			dypbt_thres_day[i] = ratio * core->mdns_ratio.dypbt_thres_def_day[i] / 128;
			dywei_max_day[i] = ratio * (core->mdns_ratio.dywei_max_def_day[i] - 10) / 128 + 10;
			dywei_min_day[i] = ratio * (core->mdns_ratio.dywei_min_def_day[i]) / 128;
			dysad_thres_night[i] = ratio * core->mdns_ratio.dysad_thres_def_night[i] / 128;
			dysta_thres_night[i] = ratio * core->mdns_ratio.dysta_thres_def_night[i] / 128;
			dypbt_thres_night[i] = ratio * core->mdns_ratio.dypbt_thres_def_night[i] / 128;
			dywei_max_night[i] = ratio * (core->mdns_ratio.dywei_max_def_night[i] - 10) / 128 + 10;
			dywei_min_night[i] = ratio * (core->mdns_ratio.dywei_min_def_night[i]) / 128;
		} else {
			dysad_thres_day[i] = (ratio - 128) * (64 - core->mdns_ratio.dysad_thres_def_day[i]) / 128 + core->mdns_ratio.dysad_thres_def_day[i];
			dysta_thres_day[i] = (ratio - 128) * (64 - core->mdns_ratio.dysta_thres_def_day[i]) / 128 + core->mdns_ratio.dysta_thres_def_day[i];
			dypbt_thres_day[i] = (ratio - 128) * (64 - core->mdns_ratio.dypbt_thres_def_day[i]) / 128 + core->mdns_ratio.dypbt_thres_def_day[i];
 			dywei_max_day[i] = (ratio - 128) * (250 - core->mdns_ratio.dywei_max_def_day[i]) / 128 + core->mdns_ratio.dywei_max_def_day[i];
 			dywei_min_day[i] = (ratio - 128) * (200 - core->mdns_ratio.dywei_min_def_day[i]) / 128 + core->mdns_ratio.dywei_min_def_day[i];
			dysad_thres_night[i] = (ratio - 128) * (64 - core->mdns_ratio.dysad_thres_def_night[i]) /
				128 + core->mdns_ratio.dysad_thres_def_night[i];
			dysta_thres_night[i] = (ratio - 128) * (64 - core->mdns_ratio.dysta_thres_def_night[i]) /
				128 + core->mdns_ratio.dysta_thres_def_night[i];
			dypbt_thres_night[i] = (ratio - 128) * (64 - core->mdns_ratio.dypbt_thres_def_night[i]) /
				128 + core->mdns_ratio.dypbt_thres_def_night[i];
			dywei_max_night[i] = (ratio - 128) * (250 - core->mdns_ratio.dywei_max_def_night[i]) /
				128 + core->mdns_ratio.dywei_max_def_night[i];
			dywei_min_night[i] = (ratio - 128) * (200 - core->mdns_ratio.dywei_min_def_night[i]) /
				128 + core->mdns_ratio.dywei_min_def_night[i];
		}
	}

	if(core_tuning->day_night == 0){
		tisp_mdns_param_array_set(&core->mdns, TISP_PARAM_MDSN_Y_SAD_THRES_ARRAY,dysad_thres_day, &size);
		tisp_mdns_param_array_set(&core->mdns, TISP_PARAM_MDSN_Y_STA_THRES_ARRAY,dysta_thres_day, &size);
		tisp_mdns_param_array_set(&core->mdns, TISP_PARAM_MDSN_Y_PBT_THRES_ARRAY,dypbt_thres_day, &size);
		tisp_mdns_param_array_set(&core->mdns, TISP_PARAM_MDSN_Y_REF_WEI_MAX_ARRAY,dywei_max_day, &size);
		tisp_mdns_param_array_set(&core->mdns, TISP_PARAM_MDSN_Y_REF_WEI_MIN_ARRAY,dywei_min_day, &size);
	} else {
		tisp_mdns_param_array_set(&core->mdns, TISP_PARAM_MDSN_Y_SAD_THRES_ARRAY,dysad_thres_night, &size);
		tisp_mdns_param_array_set(&core->mdns, TISP_PARAM_MDSN_Y_STA_THRES_ARRAY,dysta_thres_night, &size);
		tisp_mdns_param_array_set(&core->mdns, TISP_PARAM_MDSN_Y_PBT_THRES_ARRAY,dypbt_thres_night, &size);
		tisp_mdns_param_array_set(&core->mdns, TISP_PARAM_MDSN_Y_REF_WEI_MAX_ARRAY,dywei_max_night, &size);
		tisp_mdns_param_array_set(&core->mdns, TISP_PARAM_MDSN_Y_REF_WEI_MIN_ARRAY,dywei_min_night, &size);
	}

	memcpy(tparams_day.params_data.TISP_PARAM_MDSN_Y_SAD_THRES_ARRAY,dysad_thres_day, size);
	memcpy(tparams_day.params_data.TISP_PARAM_MDSN_Y_STA_THRES_ARRAY,dysta_thres_day, size);
	memcpy(tparams_day.params_data.TISP_PARAM_MDSN_Y_PBT_THRES_ARRAY,dypbt_thres_day, size);
	memcpy(tparams_day.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MAX_ARRAY,dywei_max_day, size);
	memcpy(tparams_day.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MIN_ARRAY,dywei_min_day, size);
	memcpy(tparams_night.params_data.TISP_PARAM_MDSN_Y_SAD_THRES_ARRAY,dysad_thres_night, size);
	memcpy(tparams_night.params_data.TISP_PARAM_MDSN_Y_STA_THRES_ARRAY,dysta_thres_night, size);
	memcpy(tparams_night.params_data.TISP_PARAM_MDSN_Y_PBT_THRES_ARRAY,dypbt_thres_night, size);
	memcpy(tparams_night.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MAX_ARRAY,dywei_max_night, size);
	memcpy(tparams_night.params_data.TISP_PARAM_MDSN_Y_REF_WEI_MIN_ARRAY,dywei_min_night, size);

	return 0;
}

int tisp_s_ae_attr(tisp_core_tuning_t *core_tuning, tisp_ev_attr_t ae_attr)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;

	tisp_ae_ctrls_t ae_manual;

	ae_manual.tisp_ae_it_manual = ae_attr.manual_it;
	ae_manual.tisp_ae_sensor_integration_time = ae_attr.integration_time;

	tisp_ae_manual_set(&core->ae, ae_manual);

	return 0;
}

int tisp_g_ae_attr(tisp_core_tuning_t *core_tuning, tisp_ev_attr_t *ae_attr)
{
	return 0;
}

int tisp_g_ae_min(tisp_core_tuning_t *core_tuning, tisp_ae_ex_min_t *ae_min)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	tisp_ae_ex_min_t ae_amin;

	tisp_ae_g_min(&core->ae, &ae_amin);
	ae_min->min_it = ae_amin.min_it;
	ae_min->min_again = ae_amin.min_again;

	return 0;
}


int tisp_s_ae_min(tisp_core_tuning_t *core_tuning, tisp_ae_ex_min_t ae_min)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	int ret = 0;
	ret = tisp_ae_s_min(&core->ae, ae_min);
	if(ret < 0)
		return -1;

	return 0;
}

int tisp_g_ae_zone(tisp_core_tuning_t *core_tuning, tisp_zone_info_t *ae_zone)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;

	tisp_ae_get_y_zone(&core->ae, ae_zone);

	return 0;
}

int tisp_g_af_metric(tisp_core_tuning_t *core_tuning, unsigned int *metric)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;

	tisp_af_get_metric(&core->af, metric);

	return 0;
}

int tisp_g_af_attr(tisp_core_tuning_t *core_tuning, tisp_af_attr *af_info)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	tisp_af_get_attr(&core->af, af_info);

	return 0;
}

int tisp_s_af_attr(tisp_core_tuning_t *core_tuning, tisp_af_attr af_info)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	tisp_af_set_attr(&core->af, af_info);

	return 0;
}

void tisp_s_wb_frz(tisp_core_tuning_t *core_tuning, unsigned char frz)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	tisp_awb_set_frz(&core->awb, frz);
}

void tisp_g_wb_frz(tisp_core_tuning_t *core_tuning, unsigned char *frz)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	tisp_awb_get_frz(&core->awb, frz);
}

void tisp_s_module_control(tisp_core_tuning_t *core_tuning, tisp_module_control_t top)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	unsigned int bypass = top.key;

	bypass &= 0xffff;
	bypass |= 0x80000000;
	system_reg_write(((tisp_core_t *)core_tuning->core)->priv_data, TIZAINO_BYPASS_CON, bypass);
}

void tisp_g_module_control(tisp_core_tuning_t *core_tuning, tisp_module_control_t *top)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	unsigned int bypass;

	bypass = system_reg_read(((tisp_core_t *)core_tuning->core)->priv_data, TIZAINO_BYPASS_CON);
	bypass &= 0xffff;
	top->key = bypass;
}

void tisp_s_ev_start(tisp_core_tuning_t *core_tuning, unsigned int ev_start)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	tiziano_ae_s_ev_start(&core->ae, ev_start);
}

void tisp_s_max_again(tisp_core_tuning_t *core_tuning, unsigned int max_again)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	tiziano_ae_s_max_again(&core->ae, max_again);
}

void tisp_s_max_isp_dgain(tisp_core_tuning_t *core_tuning, unsigned int max_isp_dgain)
{
	tisp_core_t *core = (tisp_core_t *)core_tuning->core;
	tiziano_ae_s_max_isp_dgain(&core->ae, max_isp_dgain);
}
