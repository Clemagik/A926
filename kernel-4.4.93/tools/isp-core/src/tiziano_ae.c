#include <linux/kernel.h>
#include <linux/irqreturn.h>
#include <linux/dma-mapping.h>
#include <linux/init.h>
#include <linux/module.h>
#include "isp-debug.h"
#include "fix_point_calc.h"
#include "../inc/tiziano_core.h"
#include "../inc/system_sensor_drv.h"
#include "../inc/tiziano_sys.h"
#include "tiziano_core.h"
#include "../inc/tiziano_isp.h"
#include "tiziano_event.h"
#include "tiziano_ae.h"
#include "tiziano_map.h"
#include "tiziano_gib.h"

#define TISP_AE_PROCESS_THREAD

/********Ae Adjust Param*********/
/* int _ae_parameter[38];   //init : JZ_Isp_Ae_Reg2par(_ae_parameter, _convert_parameter_reg); */
int _ae_parameter[38] = {
	1, 15, 1, 15,      //v_start, zone_v_num, h_start, zone_h_num
	64,64,64,64,64,64,64,64,64,64,64,64,64,64,64, //h_zone pixel number
	36,36,36,36,36,36,36,36,36,36,36,36,36,36,36, //v_zone pixel number
	30, 230, //dark threshold, saturation threshold
	1, 1     //ae frequency, histogram frequency
};
/* --> ae_static_parameter[38] */

#if 1
int ae_switch_night_mode[4]={0, 0, 23250, 15000}; //night_mode, night_mode_pre, night_mode_on_th, night_mode_off_th
/* en 1 ?? */
uint32_t _AePointPos[2] = {10, 1024}; //ae,awb
int _exp_parameter[11] = {
	250, 10, 983, 31, 4095,  //exposure parameter: alloc_ev(exp_ag)_min_th, max_exp_cnt_th, max_exp_time, max_ag*2, max_dg 4095
	4, 6,                 //max white violation frame, max dark violation frame
	0,20,0,0                  //ae_speed_en,ae_speed[0,128],P_weight_en,ae_count,
};
/* --> ae_exp_parameter[11] */
/* en 2,3; manual exp&ag&dg  6,7,8 */
uint32_t ae_ev_step[5] = {9831, 3277, 262, 65536, 32768}; //_tw_step_s(0.15*2^24), _tw_step_d(0.05*2^24), _min_step(0.004*2^24), slow_th_d(1*2^24), slow_th_s(0.5*2^24)
/* ---> ae_ev_step[5] */
uint32_t ae_stable_tol[4] = {87163, 49152, 1, 1}; //out_stable_tol_d, out_stable_tol_s, in_stable_tol_d, in_stable_tol_s
/* ae_stable_tol[4] */
int _ev_list[10] = {
	//30, 150, 1500, 7500, 15000, 22500
	35, 200, 1200, 5500, 16000, 33450, 33450,33450,33450,33450
	//20, 100, 200, 2500, 16000, 33450
};
/* --> ae_ev_list[6] */
int _lum_list[6] = {10000, 2000, 1500, 800, 500, 100};
/* --> ae_lum_list[6] */
int _at_list[10] = {
	/* 90, 84, 75, 66, 40, 30 */
	/* 90, 84, 75, 66, 40, 30 */
	/* 70, 54, 35, 26, 20, 15 */
	75, 65, 55, 45, 40, 30, 30, 30, 30, 30,
	/* 145,145,145,145,145,145 */
};
/* --> ae_target_list[6] */

int _deflicker_para[3] = {0,1,20}; //deflicker_en, min_exp_en, min_exp_tolerate
int _flicker_t[6] = {247, 494, 741, 741, 741, 741};  //{247, 494, 741, 988, 1235, 1482}
int _deflick_lut[120] = {247};
/* --> ae_deflicker_para[3] */
/* --> ae_flicker_exp[6] */
/*  en 4 */

int _scene_para[11] = { 1, 230, 414720, 30, 414720, 1, 8, 8, 1, 1, 1}; //1920 * 1080 * 0.2 =414720
/* --> ae_scene_para[11] */
/* 强光抑制使能,  de_ovexp_en, de_ovexp_th, de_ovexp_num, de_udexp_th, de_udexp_num, de_flare_dark_w, de_flare_sat_w, de_bl_dark_w, de_bl_sat_w, normal_dark_w, normal_sat_w */
uint32_t ae_scene_mode_th[4] = {163840, 163840, 163840, 163840};
/* backlight_up_pct(2.5*2^24), backlight_down_pct(2.5*2^24), */
/* flare_up_pct(2.5*2^24), flare_down_pct(2.5*2^24) */
/* ae_scene_mode_th[4] */

int _log2_lut[20] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288};
int _weight_lut[20] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
/* --> ae_roi_trend_log2_lut[20] */
/* --> ae_roi_trend_weight_lut[20] */

int _ae_zone_weight[225] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};
//--> ae_zone_weight[225]
int _scene_roui_weight[225] = {
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 4, 0, 0, 0, 0, 0, 4, 8, 8, 8, 8,
	8, 8, 8, 4, 0, 0, 0, 0, 0, 0, 0, 4, 8, 8, 8,
	8, 8, 4, 4, 0, 0, 0, 0, 0, 0, 0, 4, 8, 8, 8,
	8, 6, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 8,
	6, 6, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 6,
	6, 6, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 6,
	6, 5, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 6,
	6, 5, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 6,
	6, 5, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 6,
	6, 5, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 6,
	6, 5, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 6
};
// --> ae_scene_roui_weight[225]
int _scene_roi_weight[225] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 4, 8, 8, 8, 8, 8, 4, 0, 0, 0, 0,
	0, 0, 0, 4, 8, 8, 8, 8, 8, 8, 8, 4, 0, 0, 0,
	0, 0, 4, 4, 8, 8, 8, 8, 8, 8, 8, 4, 4, 0, 0,
	0, 2, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 2, 0,
	2, 2, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 2, 2,
	2, 2, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 2, 2,
	2, 3, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 3, 2,
	2, 3, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 3, 2,
	2, 3, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 3, 2,
	2, 3, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 3, 2,
	2, 3, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 3, 2
};
/* --> ae_scene_roi_weight[225] */

/* fine once */
uint32_t _ae_result[4] = {1, 65536, 983, 65536};  //{16, 1*2^24, 983, 1*2^24}  ag_r,ae->dg_new,exp_t,ag_new
int _ae_stat[5] = {66, 0, 128, 0, 0}; //ae_target_o, ae_stable, zone_wmean, ae_scene, exp_max_cnt
int _ae_wm_q[15] = {512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512};
#endif

/***********************AE ALGORITHM****************************/
int tisp_set_sensor_integration_time(tisp_ae_t *ae, uint32_t value)
{
	tisp_core_t *core = ae->core;
	if (ae->tisp_ae_ctrls.tisp_ae_manual || ae->tisp_ae_ctrls.tisp_ae_it_manual) {
		core->sensor_ctrl_ops->set_integration_time(&core->sensor_ctrl, ae->tisp_ae_ctrls.tisp_ae_sensor_integration_time, NULL);
	} else {
		ae->tisp_ae_ctrls.tisp_ae_sensor_integration_time =  value;
		core->sensor_ctrl_ops->set_integration_time(&core->sensor_ctrl, value, NULL);
	}

	return 0;
}

uint32_t tisp_math_exp2(uint32_t val, const unsigned char shift_in, const unsigned char shift_out);
uint32_t tisp_log2_fixed_to_fixed(const uint32_t val, const int in_fix_point, const uint8_t out_fix_point);

uint32_t tisp_set_sensor_analog_gain(tisp_ae_t *ae, uint32_t value)
{
	/*ISP_ERROR("reg = 0x%x, value = 0x%x\n", sensor_val.reg_num, sensor_val.value);*/
	/* value is 22.10 fix point */
	uint32_t wantgain;
	uint32_t wantgaindb;
	uint32_t realgain;
	uint32_t realgaindb;
	uint32_t realvalue;
	sensor_context_t sctx;
	tisp_core_t *core = ae->core;

	wantgain = value<<(16-10);
	wantgaindb = tisp_log2_fixed_to_fixed(wantgain, 16, 16);
#if 0
	realgaindb = core->sensor_ctrl_ops->alloc_analog_gain(&core->sensor_ctrl,wantgaindb, &sctx);
	realgain = tisp_math_exp2(realgaindb, 16, 16);
	realvalue = realgain >> (16-10);
#endif

	core->sensor_ctrl_ops->set_analog_gain(&core->sensor_ctrl, wantgaindb, NULL);

	core->sensor_ctrl_ops->get_analog_gain(&core->sensor_ctrl, &realgaindb, NULL);
	realgain = tisp_math_exp2(realgaindb, 16, 16);
	realvalue = realgain >> (16-10);

#if 0
	ISP_ERROR("%s,%d: value = %d, wantgain = %d, getgain = %d, realvalue = %d\n",
	       __func__, __LINE__, value, wantgain, realgain, realvalue);
#endif

	return realvalue;
}

uint32_t tisp_set_sensor_digital_gain(tisp_ae_t *ae, uint32_t value)
{
	/*ISP_ERROR("reg = 0x%x, value = 0x%x\n", sensor_val.reg_num, sensor_val.value);*/
	/* value is 22.10 fix point */
	uint32_t wantgain;
	uint32_t wantgaindb;
	uint32_t realgain;
	uint32_t realgaindb;
	uint32_t realvalue;
	sensor_context_t sctx;
	tisp_core_t *core = ae->core;

	wantgain = value << (16 - 10);
	wantgaindb = tisp_log2_fixed_to_fixed(wantgain, 16, 16);
#if 0
	realgaindb = core->sensor_ctrl_ops->alloc_digital_gain(&core->sensor_ctrl, wantgaindb, &sctx);
#endif
	core->sensor_ctrl_ops->set_digital_gain(&core->sensor_ctrl, sctx.dgain, NULL);

	core->sensor_ctrl_ops->get_analog_gain(&core->sensor_ctrl, &realgaindb, NULL);
	realgain = tisp_math_exp2(realgaindb, 16, 16);
	realvalue = realgain >> (16 - 10);
#if 0
	ISP_ERROR("%s,%d: value = %d, Dwantgain = %d, Dgetgain = %d, realvalue = %d, realgaindb = %d,wantgaindb = %d\n", __func__, __LINE__, value, wantgain, realgain, realvalue, realgaindb, wantgaindb);
#endif

	return realvalue;
}

void JZ_Isp_Ae_Reg2par(tisp_ae_t *ae, int *para, unsigned int *reg)
{
	unsigned int ae_zone_v_start, ae_zone_h_start;
	unsigned int ae_zone_v_num, ae_zone_h_num;
	unsigned int ae_zone_h[16], ae_zone_v[16];
	unsigned int ae_dark_thresh, ae_sat_thresh;
	unsigned int ae_freq, hist_freq;
	int i, j;

	ae_zone_v_start = *reg & 0x7ff;
	ae_zone_v_num = (*reg & 0xf000) >> 12;
	ae_zone_h_start = (*reg & 0x7ff0000) >> 16;
	ae_zone_h_num = (*reg & 0xf0000000) >> 28;

	for (i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++){
			ae_zone_h[i * 4 + j] = (*(reg + 1 + i) & (0x7f << (j * 8))) >> (j * 8);
			ae_zone_v[i * 4 + j] = (*(reg + 5 + i) & (0x7f << (j * 8))) >> (j * 8);
		}
	}

	ae_dark_thresh = *(reg + 9) & 0xff;
	ae_sat_thresh  = (*(reg + 9) & 0xff00) >> 8;

	ae_freq = (*(reg + 9) & 0xf0000) >> 16;
	hist_freq = (*(reg + 9) & 0xf00000) >> 20;

	*(para) = ae_zone_v_start;
	*(para + 1) = ae_zone_v_num;
	*(para + 2) = ae_zone_h_start;
	*(para + 3) = ae_zone_h_num;

	for (i = 0; i < 15; i++) {
		*(para + 4 + i) = ae_zone_h[i];
		*(para + 19 + i) = ae_zone_v[i];
	}

	*(para + 34) = ae_dark_thresh;
	*(para + 35) = ae_sat_thresh;
	*(para + 36) = ae_freq;
	*(para + 37) = hist_freq;
}

void JZ_Isp_Ae_Dg2reg(uint32_t AePointPos, unsigned int dg_reg[], uint32_t dg, uint32_t wb_static[])
{
	unsigned int dg_r, dg_b, dg_g;

	dg_r = (2 * fix_point_mult2_32(AePointPos, *(wb_static), dg) + 1) / 2;
	dg_b = (2 * fix_point_mult2_32(AePointPos, *(wb_static + 1), dg) + 1) / 2;
	dg_g = (2 * dg + 1) / 2;

	*(dg_reg)     = (dg_g << 16) | dg_r;
	*(dg_reg + 1) = (dg_b << 16) | dg_g;
}

/**************AE ALGORITHM******************/
void printf_func(tisp_ae_t *ae)
{
	static int mm=0;
	unsigned int  i,j,pr_num;
	unsigned int *sum_static;

	mm++;
	if (mm < 35000 && mm > 0) {
		ISP_ERROR("mm %d\n", mm);
		for(pr_num = 1; pr_num <= 3; pr_num++){
			switch (pr_num) {
			case 1:
				sum_static = ae->ae_array_d;
				ISP_ERROR("d\n");
				break;
			case 2:
				sum_static = ae->ae_array_m;
				ISP_ERROR("m\n");
				break;
			case 3:
				sum_static = ae->ae_array_s;
				ISP_ERROR("s\n");
				break;
			case 4:
				sum_static = ae->ae_array_dc;
				ISP_ERROR("dc\n");
				break;
			case 5:
				sum_static = ae->ae_array_sc;
				ISP_ERROR("sc\n");
				break;
			case 6:
				sum_static = ae->ae_hist_array;
				ISP_ERROR("hist\n");
				break;
			case 7:
				sum_static = ae->ae_hist_ir_array;
				ISP_ERROR("ir_hist\n");
				break;
			case 8:
				sum_static = ae->ae_array_ir;
				ISP_ERROR("ir\n");
				break;
			default:
				break;
			}
#if 1
			for (i = 0; i < 15; i++) {
				for (j = 0; j < 15; j++)
					ISP_ERROR("%d ", *(sum_static + i * 15 + j));
				ISP_ERROR("\n");
			}
#else
			for (i = 0; i < 16; i++) {
				for (j = 0; j < 16; j++)
					ISP_ERROR("%d ", *(sum_static + i * 15 + j));
				ISP_ERROR("\n");
			}
#endif
			ISP_ERROR("\n");
		}
	}
}

void tisp_ae_get_statistics(tisp_ae_t *ae, unsigned int *img, unsigned int zone_num_start)
{
	unsigned int h_zone = zone_num_start >> 28;
	unsigned int v_zone = (zone_num_start & 0xf000) >> 12;
	int i, j;

	for (i = 0; i < v_zone; i++) {
		for (j = 0; j < h_zone; j++) {
			ae->ae_array_d[i * 15 + j] = *(img + i * h_zone * 4 + j * 4) & 0x1fffff;
			ae->ae_array_m[i * 15 + j] = (((*(img + i * h_zone * 4 + j * 4 + 1)) & 0x3ff) << 11) |
				(*(img + i * h_zone * 4 + j * 4) >> 21);
			ae->ae_array_s[i * 15 + j] = (((*(img + i * h_zone * 4 + j * 4 + 1)) & 0x7ffffc00)
						  >> 10);
			ae->ae_array_dc[i * 15 + j] = (((*(img + i * h_zone * 4 + j * 4 + 1)) & 0x80000000)
						   >> 31) | (((*(img + i * h_zone * 4 + j * 4 + 2))
							      & 0xfff) << 1);
			ae->ae_array_sc[i * 15 + j] = (((*(img + i * h_zone * 4 + j * 4 + 2)) & 0x1fff000)
						   >> 12);
			ae->ae_array_ir[i * 15 + j] = (((*(img + i*h_zone*4 + j*4+2)) & 0xfe000000) >> 25)
				| (((*(img + i * h_zone * 4 + j * 3)) & 0x3fff) >> 21);
		}
	}

#ifdef PARAMETER_DUMP
	if(running == 2){
		running = 0;
		FILE *fp_d = NULL;
		FILE *fp_m = NULL;
		FILE *fp_s = NULL;
		FILE *fp_dc = NULL;
		FILE *fp_sc = NULL;
		FILE *fp_ir = NULL;

		if((fp_d = fopen("fpga_ae_pix_dark.dat","wb")) == NULL){
			printf("open save file error! \n");
		}
		if((fp_m = fopen("fpga_ae_mid_sum.dat","wb")) == NULL){
			printf("open save file error! \n");
		}
		if((fp_s = fopen("fpga_ae_pix_sat.dat","wb")) == NULL){
			printf("open save file error! \n");
		}
		if((fp_dc = fopen("fpga_ae_pix_dark_cnt.dat","wb")) == NULL){
			printf("open save file error! \n");
		}
		if((fp_sc = fopen("fpga_ae_pix_sat_cnt.dat","wb")) == NULL){
			printf("open save file error! \n");
		}
		if((fp_ir = fopen("fpga_ae_pix_ir_sum.dat","wb")) == NULL){
			printf("open save file error! \n");
		}
		char ch[9];
		for (i = 0; i < 15*15; i++) {
			snprintf(ch, sizeof(ch), "%06x,\n", ae->ae_array_d[i]);
			fwrite(ch, sizeof(ch), 1, fp_d);
			snprintf(ch, sizeof(ch), "%06x,\n", ae->ae_array_m[i]);
			fwrite(ch, sizeof(ch), 1, fp_m);
			snprintf(ch, sizeof(ch), "%06x,\n", ae->ae_array_s[i]);
			fwrite(ch, sizeof(ch), 1, fp_s);
			snprintf(ch, sizeof(ch), "%06x,\n", ae->ae_array_dc[i]);
			fwrite(ch, sizeof(ch), 1, fp_dc);
			snprintf(ch, sizeof(ch), "%06x,\n", ae->ae_array_sc[i]);
			fwrite(ch, sizeof(ch), 1, fp_sc);
			snprintf(ch, sizeof(ch), "%06x,\n", ae->ae_array_ir[i]);
			fwrite(ch, sizeof(ch), 1, fp_ir);

		}
		fclose(fp_d);
		fclose(fp_m);
		fclose(fp_s);
	}
#endif
}

void tisp_ae_get_hist(tisp_ae_t *ae, unsigned int *img, unsigned int hist_en, unsigned int hist_ir_en)
{
	unsigned int i;
	unsigned long flags;
	unsigned int tisp_ae_sum_static;

	for (i = 0; i < 128; i++) {
		if (hist_en == 1) {
			ae->ae_hist_array[i * 2] = *(img + i * 2) & 0xfffff;
			ae->ae_hist_array[i * 2 + 1] = *(img + i * 2 + 1) & 0xfffff;
			if (hist_ir_en == 1) {
				ae->ae_hist_ir_array[i * 2] = *(img + 256 + i * 2) & 0xfffff;
				ae->ae_hist_ir_array[i * 2 + 1] = *(img + 256 + i * 2 + 1) & 0xfffff;
			}
		} else if (hist_ir_en == 1){
			ae->ae_hist_ir_array[i * 2] = *(img + i * 2) & 0xfffff;
			ae->ae_hist_ir_array[i * 2 + 1] = *(img + i * 2 + 1) & 0xfffff;
		}
	}

#ifdef PARAMETER_DUMP
	if (running == 3) {
		running = 0;
		FILE *fp = NULL;
		FILE *fp1 = NULL;
		if (hist_en == 1) {
			if ((fp = fopen("fpga_hist.dat","wb")) == NULL) {
				printf("open save file error! \n");
			}
		}
		if (hist_ir_en == 1) {
			if ((fp1 = fopen("fpga_ir_hist.dat","wb")) == NULL) {
				printf("open save file error! \n");
			}
		}
		char ch[9];
		for (i = 0; i < 256; i++) {
			if (hist_en == 1) {
				snprintf(ch, sizeof(ch), "%06x,\n", ae->ae_hist_array[i]);
				fwrite(ch, sizeof(ch), 1, fp);
				/*printf("%s\n", ch);*/
			}
			if (hist_ir_en == 1) {
				snprintf(ch, sizeof(ch), "%06x,\n", ae->ae_hist_array[i]);
				fwrite(ch, sizeof(ch), 1, fp1);
				/*printf("%s\n", ch);*/
			}
		}
		fclose(fp);
		fclose(fp1);
	}
#endif

	memcpy(ae->tisp_ae_hist.ae_hist, ae->ae_hist_array, sizeof(int) * 256);

	spin_lock_irqsave(&ae->slock_hist, flags);
	memcpy(ae->tisp_ae_hist.ae_hist_nodes, ae->tisp_ae_hist_last.ae_hist_nodes, sizeof(int)*4);
	spin_unlock_irqrestore(&ae->slock_hist, flags);
	tisp_ae_sum_static = 0;
	for(i = 0; i < 5; i++)
		ae->tisp_ae_hist.ae_hist_5bin[i] = 0;

	for (i = 0; i < ae->tisp_ae_hist.ae_hist_nodes[0]; i++) {
		ae->tisp_ae_hist.ae_hist_5bin[0] += ae->ae_hist_array[i];
	}
	tisp_ae_sum_static += ae->tisp_ae_hist.ae_hist_5bin[0];
	for (i = ae->tisp_ae_hist.ae_hist_nodes[0]; i < ae->tisp_ae_hist.ae_hist_nodes[1]; i++) {
		ae->tisp_ae_hist.ae_hist_5bin[1] += ae->ae_hist_array[i];
	}
	tisp_ae_sum_static += ae->tisp_ae_hist.ae_hist_5bin[1];
	for (i = ae->tisp_ae_hist.ae_hist_nodes[1]; i < ae->tisp_ae_hist.ae_hist_nodes[2]; i++) {
		ae->tisp_ae_hist.ae_hist_5bin[2] += ae->ae_hist_array[i];
	}
	tisp_ae_sum_static += ae->tisp_ae_hist.ae_hist_5bin[2];
	for (i = ae->tisp_ae_hist.ae_hist_nodes[2]; i < ae->tisp_ae_hist.ae_hist_nodes[3]; i++) {
		ae->tisp_ae_hist.ae_hist_5bin[3] += ae->ae_hist_array[i];
	}
	tisp_ae_sum_static += ae->tisp_ae_hist.ae_hist_5bin[3];
	for (i = ae->tisp_ae_hist.ae_hist_nodes[3]; i <= 255; i++) {
		ae->tisp_ae_hist.ae_hist_5bin[4] += ae->ae_hist_array[i];
	}
	tisp_ae_sum_static += ae->tisp_ae_hist.ae_hist_5bin[4];

	if (tisp_ae_sum_static == 0) {
		ae->tisp_ae_hist.ae_hist_5bin[0] = 65535;
		ae->tisp_ae_hist.ae_hist_5bin[1] = 0;
		ae->tisp_ae_hist.ae_hist_5bin[2] = 0;
		ae->tisp_ae_hist.ae_hist_5bin[3] = 0;
		ae->tisp_ae_hist.ae_hist_5bin[4] = 0;
	} else {
		for (i = 1; i < 5; i++) {
			ae->tisp_ae_hist.ae_hist_5bin[i] = fix_point_div(
				0, fix_point_mult2(0, (uint64_t)ae->tisp_ae_hist.ae_hist_5bin[i], (uint64_t)65535),
				tisp_ae_sum_static);
		}
		ae->tisp_ae_hist.ae_hist_5bin[0] = 65535 - ae->tisp_ae_hist.ae_hist_5bin[1] -
			ae->tisp_ae_hist.ae_hist_5bin[2] - ae->tisp_ae_hist.ae_hist_5bin[3] -
			ae->tisp_ae_hist.ae_hist_5bin[4];
	}

	spin_lock_irqsave(&ae->slock_hist, flags);
	memcpy(ae->tisp_ae_hist_last.ae_hist, ae->tisp_ae_hist.ae_hist, sizeof(int) * 256);
	memcpy(ae->tisp_ae_hist_last.ae_hist_5bin, ae->tisp_ae_hist.ae_hist_5bin, sizeof(int) * 5);
	spin_unlock_irqrestore(&ae->slock_hist, flags);
}

int tisp_ae_get_hist_custome(tisp_ae_t *ae, tisp_ae_sta_t *ae_hist)
{
	unsigned long flags;

	spin_lock_irqsave(&ae->slock_hist, flags);
	memcpy(ae_hist,&ae->tisp_ae_hist_last,sizeof(tisp_ae_sta_t));
	spin_unlock_irqrestore(&ae->slock_hist, flags);

	return 0;
}

int tisp_ae_set_hist_custome(tisp_ae_t *ae, tisp_ae_sta_t ae_hist)
{
	unsigned long flags;

	spin_lock_irqsave(&ae->slock_hist, flags);
	memcpy(ae->tisp_ae_hist_last.ae_hist_nodes, ae_hist.ae_hist_nodes, sizeof(int) * 4);
	spin_unlock_irqrestore(&ae->slock_hist, flags);

	return 0;
}

int AePweightCalculate(int AePointPos, int zone_cnt, int zone_p_cnt,
		       int ds_th, int *log2_lut, int *weight_lut)
{
	int P_weight = 1;
	uint32_t log2 = 0;

	if (zone_cnt == 0) {
		P_weight = 0;
	} else {
#if 0
		log2 = fix_point_div_32(AePointPos, (uint32_t)(ds_th * zone_p_cnt) << AePointPos, (uint32_t)zone_cnt << AePointPos);
		if (log2 >= ((uint32_t)log2_lut[19]<<AePointPos)) {
			P_weight = fix_point_div_32(AePointPos, (uint32_t)(zone_cnt * weight_lut[19]) << AePointPos, (uint32_t)zone_p_cnt << AePointPos);
		} else {
			m=0;
			while (!(log2 >= ((uint32_t)log2_lut[m]<<AePointPos) && log2 < ((uint32_t)log2_lut[m+1]<<AePointPos))) {
				m++;
			}

			P_weight = fix_point_mult2_32(AePointPos, (uint32_t)(weight_lut[m+1] - weight_lut[m])<< AePointPos, log2 - ((uint32_t)log2_lut[m]<< AePointPos));
			P_weight = ((uint32_t)weight_lut[m]<<AePointPos) + fix_point_div_32(AePointPos, P_weight, (uint32_t)(log2_lut[m+1] - log2_lut[m]) << AePointPos);
			P_weight = fix_point_mult2_32(AePointPos, P_weight,
						      fix_point_div_32(AePointPos, (uint32_t)zone_cnt << AePointPos, (uint32_t)zone_p_cnt << AePointPos));
		}
#endif

#if 1
		log2 = fix_point_div_32(AePointPos,
					(uint32_t)(ds_th * zone_p_cnt) << AePointPos,
					(uint32_t)zone_cnt << AePointPos); // log2_x 22.10
		log2 = tisp_log2_fixed_to_fixed(log2, AePointPos, AePointPos); // log2_y 22.10
		P_weight = fix_point_div_32(AePointPos, fix_point_mult2_32(AePointPos, (uint32_t)zone_cnt<<AePointPos, log2), (uint32_t)zone_p_cnt << AePointPos);
#endif
	}

	return P_weight;
}

void ae_weight_mean2(tisp_ae_t *ae, int dark_weight,
		     int sat_weight, int *zone_wmean, uint32_t *th_pct,
		     int *pic_ysum, int *scene_roi_wmean, int *scene_roui_wmean)
{
	int *dark_sum = ae->IspAeWmeanParam.a_ae_array_d;
	int *mid_sum = ae->IspAeWmeanParam.a_ae_array_m;
	int *sat_sum = ae->IspAeWmeanParam.a_ae_array_s;
	int *dark_cnt = ae->IspAeWmeanParam.a_ae_array_dc;
	int *sat_cnt = ae->IspAeWmeanParam.a_ae_array_sc;
	int *ae_parameter = ae->IspAeWmeanParam.a_ae_parameter;
	int *ae_zone_weight = ae->IspAeWmeanParam.a_ae_zone_weight;
	int *exp_parameter = ae->IspAeWmeanParam.a_exp_parameter;
	int *ae_stat = ae->IspAeWmeanParam.a_ae_stat;
	int *scene_roui_weight = ae->IspAeWmeanParam.a_scene_roui_weight;
	int *scene_roi_weight = ae->IspAeWmeanParam.a_scene_roi_weight;
	int *log2_lut = ae->IspAeWmeanParam.a_log2_lut;
	int *weight_lut = ae->IspAeWmeanParam.a_weight_lut;
	uint32_t AePointPos = ae->IspAeWmeanParam.a_AePointPos[0];

	int zone_sum = 0;
	int zone_ps_cnt = 0;
	int zone_pd_cnt = 0;
	int zone_pm_cnt = 0;
	int zone_p_cnt = 0;
	unsigned long flags = 0;

	int ps_sum = 0;
	int pd_sum = 0;
	int p_sum = 0;
	int y_sum = 0;
	int weight_sum = 0;

	int h_zone = ae_parameter[3];
	int v_zone = ae_parameter[1];
	int dark_th = ae_parameter[34];
	int sat_th = ae_parameter[35];

	int ae_stable = *(ae_stat + 1);
	int Pd_weight = 1,Pm_weight = 1,Ps_weight = 1, P_weight = 1;

	int i = 0, j = 0;

	int scene_roi_sum = 0;
	int scene_roi_w_sum = 0;
	int scene_roui_sum = 0;
	int scene_roui_w_sum = 0;

	int zone_pixel_num = 0;
	uint32_t zone_gray_sum = 0;

	for (i = 0; i < v_zone; i++) {
		for (j = 0; j < h_zone; j++) {
			/*****************************************************************/
			zone_pixel_num = ae_parameter[19 + i] * ae_parameter[4 + j];
			zone_gray_sum = dark_sum[i * h_zone + j] +
				mid_sum[i * h_zone + j] +
				sat_sum[i * h_zone + j];
			ae->y_zone[i][j] = zone_gray_sum / zone_pixel_num;
			*pic_ysum = *pic_ysum + zone_gray_sum / zone_pixel_num;

			/*****************************************************************/
			scene_roi_sum = scene_roi_sum +
				zone_gray_sum  * scene_roi_weight[i * h_zone + j];
			scene_roi_w_sum = scene_roi_w_sum +
				zone_pixel_num * scene_roi_weight[i * h_zone + j];
			scene_roui_sum = scene_roui_sum +
				zone_gray_sum * scene_roui_weight[i*h_zone + j];
			scene_roui_w_sum = scene_roui_w_sum +
				zone_pixel_num * scene_roui_weight[i * h_zone + j];

			/*****************************************************************/
			zone_ps_cnt = sat_cnt[i * h_zone + j]  * sat_weight;
			zone_pd_cnt = dark_cnt[i * h_zone + j] * dark_weight;
			zone_pm_cnt = zone_pixel_num - (sat_cnt[i * h_zone + j] +
							dark_cnt[i * h_zone + j]);
			zone_p_cnt = zone_pd_cnt + zone_pm_cnt + zone_ps_cnt;
			zone_sum = dark_sum[i * h_zone + j] * dark_weight +
				mid_sum[i * h_zone + j] +
				sat_sum[i * h_zone + j] * sat_weight;
			ps_sum = zone_ps_cnt + ps_sum;
			pd_sum = zone_pd_cnt + pd_sum;
			p_sum = zone_p_cnt  + p_sum;
			/**********************************************/
			if (exp_parameter[9] == 0 || ae_stable == 1) { //0
				P_weight=1;
			} else {
				Pd_weight = AePweightCalculate(AePointPos, zone_pd_cnt,
							       zone_p_cnt, dark_th + 1,
							       log2_lut, weight_lut);
				Pm_weight = AePweightCalculate(AePointPos, zone_pm_cnt,
							       zone_p_cnt, sat_th - dark_th - 1,
							       log2_lut, weight_lut);
				Ps_weight = AePweightCalculate(AePointPos, zone_ps_cnt,
							       zone_p_cnt, 255 - sat_th + 1,
							       log2_lut, weight_lut);
				P_weight = Pd_weight + Pm_weight + Ps_weight;
				P_weight = ((fix_point_mult2_32(AePointPos, P_weight, P_weight))
					    >> AePointPos) / 4 + 1;
				/* ISP_ERROR("%d ", P_weight); */
			}
			/*******************************************/
			y_sum = y_sum + zone_sum * ae_zone_weight[i * h_zone + j] * P_weight;
			weight_sum = weight_sum +
				zone_p_cnt * ae_zone_weight[i * h_zone + j] *
				P_weight;
		}
		/* ISP_ERROR("\n"); */
	}
	/* ISP_ERROR("\n"); */
	spin_lock_irqsave(&ae->slock, flags);
	memcpy(&ae->y_zone_last,&ae->y_zone,sizeof(ae->y_zone));
	spin_unlock_irqrestore(&ae->slock, flags);

	*scene_roi_wmean = scene_roi_sum / scene_roi_w_sum;
	*scene_roui_wmean = scene_roui_sum / scene_roui_w_sum;

	*zone_wmean = y_sum / weight_sum;
	if(*zone_wmean <= 0)
		*zone_wmean = 1;

	th_pct[0] = fix_point_div_32(AePointPos,
				     (uint32_t)ps_sum<<AePointPos,
				     (uint32_t)p_sum<<AePointPos);
	th_pct[1] = fix_point_div_32(AePointPos,
				     (uint32_t)pd_sum<<AePointPos,
				     (uint32_t)p_sum<<AePointPos);
	/* printf_func(); */
}

void tisp_ae_tune(uint32_t *th_pct, uint32_t *tw_step_s,
		  uint32_t *tw_step_d, uint32_t ae_speed,
		  uint32_t AePointPos, uint32_t u32AePointPosValue)
{
	uint32_t sat_ratio =0;
	uint32_t dark_ratio=0;

	if ((th_pct[0] + *tw_step_s) <= u32AePointPosValue) // (th_pct + tw_step_s:0.15) <= 1
		sat_ratio = th_pct[0];
	else
		sat_ratio = u32AePointPosValue - *tw_step_s;

	if ((th_pct[1] + *tw_step_d) <= u32AePointPosValue) // (th_pct + tw_step_d:0.05) <= 1
		dark_ratio = th_pct[1];
	else
		dark_ratio = u32AePointPosValue - *tw_step_d;

	*tw_step_s = *tw_step_s + fix_point_div_32(AePointPos,
						   fix_point_mult3_32(AePointPos,
								      (uint32_t)ae_speed<<AePointPos,
								      sat_ratio, sat_ratio),
						   (uint32_t)128<<AePointPos);
	*tw_step_d = *tw_step_d + fix_point_div_32(AePointPos,
						   fix_point_mult3_32(AePointPos,
								      (uint32_t)ae_speed<<AePointPos,
								      dark_ratio, dark_ratio),
						   (uint32_t)128<<AePointPos);
}

uint32_t tisp_ae_target(uint32_t ev_in, uint32_t *ev_list, uint32_t *at_list, uint32_t AePointPos)
{
    uint32_t i=0, lum_num=0;
    uint32_t ae_target = 128;

	if (ev_in <= ((uint32_t)ev_list[0] << AePointPos)) {
		ae_target = at_list[0];
	} else if (ev_in >= ((uint32_t)ev_list[9] << AePointPos)) {
		ae_target = at_list[9];
	} else {
		for (i = 0; i < 9; i++) {
			if ((((uint32_t)ev_list[i] << AePointPos) <= ev_in) && ( ((uint32_t)ev_list[i + 1] << AePointPos) >= ev_in)) {
				lum_num = i;
				break;
			}
		}

		ae_target = ISPINT((ev_in >> AePointPos),
				   (uint32_t)(ev_list[lum_num]),
				   (uint32_t)(ev_list[lum_num + 1]),
				   (uint32_t)(at_list[lum_num]),
				   (uint32_t)(at_list[lum_num+1]));
		/* ISP_ERROR("ae_target ISPINT is %d\n",ae_target); */
		/* unsigned int tmp = fix_point_div_32(AePointPos,ISPSUBABS(ev_in,((uint32_t)ev_list[lum_num]<<AePointPos)),ISPSUBABS((uint32_t)(ev_list[lum_num] << AePointPos),(uint32_t)(ev_list[lum_num+1] << AePointPos))); */
		/* unsigned int ae_target1 = fix_point_mult2_32(AePointPos,tmp,(ISPSUBABS((uint32_t)(at_list[lum_num]),(uint32_t)(at_list[lum_num+1])))<< AePointPos) >> AePointPos; */
		/* ae_target = ISPADDABS(ae_target1,at_list[lum_num],at_list[lum_num+1]); */
		/* ISP_ERROR("tmp is %d,ev_in is %d,target 1 %d,target is %d\n",tmp,ev_in,ae_target1,ae_target); */

	}
	/* ISP_ERROR("%s:%d::ae_target %d\n",__func__,__LINE__, ae_target); */

    return ae_target;
}

void ae_tune2(tisp_ae_t *ae, int zone_wmean,
	      uint32_t *th_pct, int ovexp_sum, int de_ovexp_num,
	      int udexp_sum, int de_udexp_num, int scene_roi_wmean,
	      int scene_roui_wmean, int *scene_para)
{
	int *exp_parameter = ae->IspAeTuneParam.a_exp_parameter;
	int *ev_list_ir = ae->IspAeTuneParam.a_ev_list;
	/* int *lum_list = ae->IspAeTuneParam.a_lum_list; */
	int *at_list_ir = ae->IspAeTuneParam.a_at_list;
	uint32_t *ae_result = ae->IspAeTuneParam.a_ae_result;
	uint32_t *ae_reg = ae->IspAeTuneParam.a_ae_reg;
	int *ae_stat = ae->IspAeTuneParam.a_ae_stat;
	int *ae_wm_q = ae->IspAeTuneParam.a_ae_wm_q;
	int *flicker_t = ae->IspAeTuneParam.a_flicker_t;
	int *deflicker_para = ae->IspAeTuneParam.a_deflicker_para;

	uint32_t tw_step_s = ae->IspAeTuneParam.a_ae_ev_step[0];  //(0.15*2^24)
	uint32_t tw_step_d = ae->IspAeTuneParam.a_ae_ev_step[1];  //(0.05*2^24)
	uint32_t min_step = ae->IspAeTuneParam.a_ae_ev_step[2];  //(0.004*2^24)
	uint32_t slow_th_d = ae->IspAeTuneParam.a_ae_ev_step[3];  //(1*2^24)
	uint32_t slow_th_s = ae->IspAeTuneParam.a_ae_ev_step[4];  //(0.5*2^24)
	uint32_t backlight_up_pct = ae->IspAeTuneParam.a_ae_scene_mode_th[0];  //(2.5*2^24)
	uint32_t backlight_down_pct= ae->IspAeTuneParam.a_ae_scene_mode_th[1];  //(2.5*2^24)
	uint32_t flare_up_pct = ae->IspAeTuneParam.a_ae_scene_mode_th[2];  //(2.5*2^24)
	uint32_t flare_down_pct = ae->IspAeTuneParam.a_ae_scene_mode_th[3];  //(2.5*2^24)
	uint32_t out_stable_tol_d = ae->IspAeTuneParam.a_ae_stable_tol[0]; // 1.33*2^24
	uint32_t out_stable_tol_s = ae->IspAeTuneParam.a_ae_stable_tol[1]; // 0.75*2^24
	uint32_t in_stable_tol_d = ae->IspAeTuneParam.a_ae_stable_tol[2]; // 1
	uint32_t in_stable_tol_s = ae->IspAeTuneParam.a_ae_stable_tol[3]; // 1
	uint32_t AePointPos = ae->IspAeTuneParam.a_AePointPos[0];
	uint32_t u32AePointPosValue= (uint32_t)1<<AePointPos;
	uint32_t *ae_IrHist = ae->IspAeTuneParam.a_ae_hist_ir_array;
	uint32_t max_deflick = *(ae->IspAeTuneParam.a_ae_nodes_num);
	uint32_t compensation = *(ae->IspAeTuneParam.a_ae_compensation);

	int deflicker = deflicker_para[0];
	int sub_df  = deflicker_para[1];
	int sub_df_tol = deflicker_para[2];
	int max_it   = exp_parameter[2];
#if 0
	uint32_t max_ag = fix_point_div_32(AePointPos, (uint32_t)exp_parameter[3]<<AePointPos, (uint32_t)2<<AePointPos);
	uint32_t max_dg = fix_point_div_32(AePointPos, (uint32_t)exp_parameter[4]<<AePointPos, (uint32_t)1024<<AePointPos);
#endif
	uint32_t max_ag = (uint32_t)exp_parameter[3];
	uint32_t max_dg = (uint32_t)exp_parameter[4];
	/* printf("max_ag is %llu %llu\n", max_ag, max_dg); //3*2^24 2*2^24  =25165824 */
	/* int vw_stable	  = exp_parameter[5]; */
	/* int vd_stable	  = exp_parameter[6]; */
	int ae_target = 0;
	uint32_t exp_t = 0; // unsigned long
	uint32_t ag = 0;
	uint32_t dg = 0;
	int zone_wmean_sm = 0;

	uint32_t ag_o = ae_result[3];
	uint32_t dg_o = ae_result[1];
	uint32_t exp_t_o = ae_result[2]; // unsigned long

	int ae_target_o = *ae_stat;
	int ae_stable = *(ae_stat + 1);
	int ae_stable_old = ae_stable;
	int ae_scene_old = *(ae_stat + 3);
	int exp_max_cnt = *(ae_stat + 4);
	int exp_max = 0;

	unsigned int scene_weight;
	/* unsigned int scene_weight_max; */

	int ag_r = 0; //, ag_r_h=0, ag_r_l=0, ag_h=0;
	/* int ag_r; */

	uint32_t ev_o = fix_point_mult3_32(AePointPos, ag_o, dg_o, (uint32_t)exp_t_o<<AePointPos);
	uint32_t ev = 0, min_ev=0, extra_ev=0, extra_max_it=0, extra_max_ag=0, extra_max_dg=0, extra_flicker_t=0;
	int i = 0;
	uint32_t k1 = 0;
	uint32_t k2 = 0;
	uint64_t k4 = 0;
	uint32_t ev_n = 0;
	int ae_scene = 0; //0:normal 1:backlight 2:flare
	int ae_speed_en = exp_parameter[7];
	int ae_speed = exp_parameter[8];
	/* uint32_t sat_ratio=0; */
	/* uint32_t dark_ratio=0; */

	int alloc_ev_min_th = exp_parameter[0];
	int max_exp_cnt_th = exp_parameter[1];
	uint32_t img_num,img_error;

	uint32_t ir_sum = 0, ir_eve = 0, ir_ev_coeff;
	int ev_list[10], at_list[10];

	static int ftune_wmeans = 0;

	tisp_core_t *core = ae->core;
	/*change _flicker_t[5] to isp min_dg*/
	ae->min_dg = ae->_flicker_t[5] >= 1024 ? ae->_flicker_t[5] : 1024;

	for (i = 0; i < 10; i++) {
		ev_list[i] = ev_list_ir[i];
		at_list[i] = at_list_ir[i];
	}

	if (tiziano_gib_config_line[3] == 1) {
		for (i = 0; i < 256; i++) {
			ir_sum = ir_sum + i * ae_IrHist[i];
		}
		ir_eve = ir_sum / (ae->_ae_parameter[4] * ae->_ae_parameter[19] *
				   ae->_ae_parameter[1] * ae->_ae_parameter[3]);
		if (ir_eve > 255)
			ir_eve = 255;
		ir_ev_coeff = fix_point_div_32(AePointPos,
					       (uint32_t)(256 - ir_eve / 2) << AePointPos,
					       (uint32_t)256 << AePointPos);
		for (i = 0;i < 10;i++) {
			ev_list[i] = (fix_point_mult2_32(AePointPos,
							 ev_list[i]<<AePointPos,
							 ir_ev_coeff))>>AePointPos;
			at_list[i] = (fix_point_mult2_32(AePointPos,
							 at_list[i]<<AePointPos,
							 ir_ev_coeff))>>AePointPos;
		}
		/* ISP_ERROR("%d %d \n", ir_eve, ir_ev_coeff); */
		{
			tisp_event_t ae_ir_event;
			ae_ir_event.type = TISP_EVENT_TYPE_AE_IR;
			ae_ir_event.data1 = ir_eve;
			tisp_event_push(&core->tisp_event_mg, &ae_ir_event);
			/* tisp_ae_ctrls.tisp_ae_ir = ir_eve; */
		}
	}

#if 0
	if (scene_para[0] == 1) {
		if (scene_para[6] <= 2)
			scene_weight = 1;
		else
			scene_weight=scene_para[6]; //de_flare_sat_w
		img_num = ae->_ae_parameter[4] * ae->_ae_parameter[19] *
			ae->_ae_parameter[1] * ae->_ae_parameter[3];
		img_error = ovexp_sum * (scene_weight - 1) / 2;
		if(img_error > img_num * 8 / 10)
			img_error = img_num * 8 / 10;
		/* ISP_ERROR("%d  %d %d \n", img_error, img_num, ae_target); */

		for (i = 0; i < 10; i++) {
			ev_list[i] = ev_list[i];
			at_list[i] = at_list[i] * (img_num - img_error) / img_num;
			if(at_list[i] <= 0)
				at_list[i] = 1;
		}
	}
#else
	if (scene_para[0] == 1) {
		scene_weight = scene_para[6]; //de_flare_sat_w
		if (scene_weight < 1)
			scene_weight = 1;
		else if (scene_weight > 64)
			scene_weight = 64;

		img_num = ae->_ae_parameter[4] * ae->_ae_parameter[19] * ae->_ae_parameter[1] * ae->_ae_parameter[3];

		img_error = ovexp_sum * (scene_weight - 1) * 2;

		zone_wmean = zone_wmean * (img_num + img_error) / img_num;

	}
#endif

	for (i = 0; i < 10; i++) {
		if(compensation==128){
			ev_list[i] = ev_list[i];
			at_list[i] = at_list[i];
		}else if(compensation>128){
			ev_list[i] = ev_list[i] * 2 * compensation / (128*2);
			at_list[i] = at_list[i] * 2 * compensation / (128*2);
		}else{
			ev_list[i] = ev_list[i] * 2 * compensation / (128*2);
			at_list[i] = at_list[i] * 2 * compensation / (128*2);
		}
		if(ev_list[i] <= 0)
			ev_list[i] = 1;
		if(at_list[i] <= 0)
			at_list[i] = 1;
	}

	/*ae_wmean*/
	for (i = 0; i < 14; i++) {
		*(ae_wm_q+i) = *(ae_wm_q + i + 1);
	}
	*(ae_wm_q + 14) = zone_wmean;

	if(ae->ftune == 1){
		ftune_wmeans = 1;
	}
	if(ae_stable == 1){
		ftune_wmeans = 0;
	}

	if(ftune_wmeans == 1){
		zone_wmean_sm = zone_wmean;
		ae_speed = 1;
	}else{
		zone_wmean_sm = (10 * (*(ae_wm_q + 14)) + 9 * (*(ae_wm_q + 13)) +
				8 * (*(ae_wm_q + 12)) + 7 * (*(ae_wm_q + 11)) +
				6 * (*(ae_wm_q + 10))) / 40;
	}
	if (zone_wmean_sm <= 0){
		zone_wmean_sm = 1;
	}

	if(ae->ftune == 1){
		if(ae->ae_ev_init_en == 1){
			ev_o = ae->ae_ev_init_strict << AePointPos;
			ae->trig_cal = 0;
			ae->ae_ev_init_en = 0;
		}
		ae_target_o = tisp_ae_target(ev_o, ev_list, at_list, AePointPos);
	}

	k1 = fix_point_div_32(AePointPos,
			(uint32_t)ae_target_o<<AePointPos,
			(uint32_t)zone_wmean_sm<<AePointPos);
	ev_n = fix_point_mult2_32(AePointPos, ev_o, k1);
	/*ISP_ERROR("1 zone_wmean:%d  zone_wmean_sm:%d  ae_target_o:%d  ev_o:%d  k1:%d\n", zone_wmean, zone_wmean_sm, ae_target_o, ev_o>>10, k1); */

	if (ae_stable == 1) {
		/* ISP_ERROR("%s:%d\n\n",__func__,__LINE__); */
		if (k1 >= out_stable_tol_d || k1<= out_stable_tol_s) {
			ae_stable = 0;                     //ae_stable: 0,
			*(ae_stat + 1) = ae_stable;
			ev = ev_o;
		} else {
			ev = ev_o;
			*(ae_stat + 2) = zone_wmean;       //last stable ae wmean
			ae_target = ae_target_o;
		}
		/* ISP_ERROR("%s:%d:: ae_stable: %d\n",__func__,__LINE__,ae_stable); */
	}

	if (ae_stable == 0) {
		/* ISP_ERROR("@@@@@%s:%d@@@@\n",__func__,__LINE__); */
		if (zone_wmean_sm >= ae_target_o - in_stable_tol_d && zone_wmean_sm <= ae_target_o + in_stable_tol_s) {
			/* ISP_ERROR("@@@@@%s:%d@@@@\n",__func__,__LINE__); */
			/* ISP_ERROR("zone is %d,ae target o is %d,
			   in stabel_tol_d is %d,tol_s is %d\n",
			   zone_wmean_sm,ae_target_o,in_stable_tol_d,
			   in_stable_tol_s); */
			ae_stable = 1;
			*(ae_stat + 1) = ae_stable;
			*(ae_stat + 2) = zone_wmean;
			ev = ev_o;
			*ae_stat = ae_target_o;
		}
		if (zone_wmean_sm < ae_target_o - in_stable_tol_d || zone_wmean_sm > ae_target_o + in_stable_tol_s || ae->force_trig == 1) {
			ae->force_trig = 0;
			/* ISP_ERROR("@@@@@%s:%d@@@@\n",__func__,__LINE__); */
			/* ISP_ERROR("zone wmean is %d,ae target o is %d,in stabel_tol_d is %d,tol_s is %d\n", zone_wmean_sm,ae_target_o,in_stable_tol_d,in_stable_tol_s); */
			/* ISP_ERROR("%s:%d\n\n",__func__,__LINE__); */
#if 1
            ae_target = tisp_ae_target(ev_n, ev_list, at_list, AePointPos);
#else
			if (ev_n <= ((uint32_t)ev_list[0] << AePointPos)) {
				ae_target = at_list[0];
				//ISP_ERROR("%s:%d::ae_target %d\n",__func__,__LINE__, ae_target);
			} else if (ev_n >= ((uint32_t)ev_list[9] << AePointPos)) {
				ae_target = at_list[9];
				//ISP_ERROR("%s:%d::ae_target %d\n",__func__,__LINE__, ae_target);
			} else {
				for (i = 0; i < 9; i++) {
					if ((((uint32_t)ev_list[i] << AePointPos) <= ev_n) && ( ((uint32_t)ev_list[i + 1] << AePointPos) >= ev_n)) {
						lum_num = i;
						break;
					}
				}

				ae_target = ISPINT((ev_n >> AePointPos),
						   (uint32_t)(ev_list[lum_num]),
						   (uint32_t)(ev_list[lum_num + 1]),
						   (uint32_t)(at_list[lum_num]),
						   (uint32_t)(at_list[lum_num+1]));
				/* ISP_ERROR("ae_target ISPINT is %d\n",ae_target); */
				/* unsigned int tmp = fix_point_div_32(AePointPos,ISPSUBABS(ev_n,((uint32_t)ev_list[lum_num]<<AePointPos)),ISPSUBABS((uint32_t)(ev_list[lum_num] << AePointPos),(uint32_t)(ev_list[lum_num+1] << AePointPos))); */
				/* unsigned int ae_target1 = fix_point_mult2_32(AePointPos,tmp,(ISPSUBABS((uint32_t)(at_list[lum_num]),(uint32_t)(at_list[lum_num+1])))<< AePointPos) >> AePointPos; */
				/* ae_target = ISPADDABS(ae_target1,at_list[lum_num],at_list[lum_num+1]); */
				/* ISP_ERROR("tmp is %d,ev_n is %d,target 1 %d,target is %d\n",tmp,ev_n,ae_target1,ae_target); */

			}
			/* ISP_ERROR("%s:%d::ae_target %d\n",__func__,__LINE__, ae_target); */
#endif
			*ae_stat = ae_target;
			if (ae_target > zone_wmean_sm) {
				k2 = fix_point_div_32(AePointPos,
						      (uint32_t)(ae_target - zone_wmean_sm) << AePointPos,
						      (uint32_t)zone_wmean_sm << AePointPos);
			} else {
				k2 = fix_point_div_32(AePointPos,
						      (uint32_t)(zone_wmean_sm - ae_target) << AePointPos,
						      (uint32_t)zone_wmean_sm << AePointPos); //-1
			}

			if (ae_speed_en == 1) {
#if 1
				tisp_ae_tune(th_pct, &tw_step_s,
					     &tw_step_d, ae_speed,
					     AePointPos, u32AePointPosValue);
#else
				/* (th_pct + tw_step_s:0.15) <= 1 */
				if ((th_pct[0] + tw_step_s) <= u32AePointPosValue)
					sat_ratio  = th_pct[0];
				else
					sat_ratio  = u32AePointPosValue - tw_step_s;
				/* (th_pct + tw_step_d:0.05) <= 1 */
				if ((th_pct[1] + tw_step_d) <= u32AePointPosValue)
					dark_ratio = th_pct[1];
				else
					dark_ratio = u32AePointPosValue - tw_step_d;

				tw_step_s = tw_step_s + fix_point_div_32(AePointPos, fix_point_mult3_32(AePointPos, (uint32_t)ae_speed<<AePointPos, sat_ratio, sat_ratio), (uint32_t)128<<AePointPos);
				tw_step_d = tw_step_d + fix_point_div_32(AePointPos, fix_point_mult3_32(AePointPos, (uint32_t)ae_speed<<AePointPos, dark_ratio, dark_ratio), (uint32_t)128<<AePointPos);
				/* printf("sat_ratio:%llu tw_step_s: %llu dark_ratio:%llu tw_step_d: %llu \n",sat_ratio,tw_step_s,dark_ratio,tw_step_d); */
#endif
			}
#if 1
			if (ae_target > zone_wmean_sm) {
				if (k2 > slow_th_d) {
					k4 = ((uint64_t)u32AePointPosValue << AePointPos) + fix_point_mult2_64(2 * AePointPos, (uint64_t)tw_step_d << AePointPos, (uint64_t)k2 << AePointPos); //k3 : 42.20
				} else {
					k4 = ((uint64_t)u32AePointPosValue << AePointPos) + fix_point_mult3_64(2 * AePointPos, (uint64_t)tw_step_d << AePointPos, (uint64_t)k2 << AePointPos, (uint64_t)k2 << AePointPos); //k3 : 42.20
				}
			} else {
				if(-1 * k2 > -1 * slow_th_s){  //0.5*2^10
					k4 = ((uint64_t)u32AePointPosValue << AePointPos) - fix_point_div_64(2 * AePointPos, fix_point_mult3_64(2 * AePointPos, (uint64_t)tw_step_s << AePointPos, (uint64_t)k2 << AePointPos, (uint64_t)k2 << AePointPos), (uint64_t)(u32AePointPosValue - k2) << AePointPos);
				} else {
					k4 = ((uint64_t)u32AePointPosValue << AePointPos) - fix_point_mult2_64(2 * AePointPos, (uint64_t)tw_step_s << AePointPos, (uint64_t)k2 << AePointPos);
				}
			}

			if (k4 < ((uint64_t)u32AePointPosValue << AePointPos) + ((uint64_t)min_step << AePointPos) && k4 > ((uint64_t)u32AePointPosValue << AePointPos))
				k4 = ((uint64_t)u32AePointPosValue<<AePointPos) + ((uint64_t)min_step<<AePointPos);
			else if (k4 > ((uint64_t)u32AePointPosValue<<AePointPos) - ((uint64_t)min_step<<AePointPos) && k4 < ((uint64_t)u32AePointPosValue<<AePointPos))
				k4 = ((uint64_t)u32AePointPosValue<<AePointPos) - ((uint64_t)min_step<<AePointPos);
			ev = (uint32_t)((fix_point_mult2_64(2*AePointPos,
							    k4, (uint64_t)ev_o<<AePointPos))>>AePointPos);
			if (ev < ((uint32_t)1<<AePointPos))
				ev=(uint32_t)1<<AePointPos;
#else
			if (ae_target > zone_wmean_sm){
				if (k2 > slow_th_d){
					k3 = u32AePointPosValue +
						fix_point_mult2_32(AePointPos, tw_step_d, k2); //k3 : 22.10
				} else {
					k3 = u32AePointPosValue +
						fix_point_mult3_32(AePointPos, tw_step_d, k2, k2); //k3 : 22.10
				}
			} else {
				if (-1 * k2 > -1 * slow_th_s){  //0.5*2^10
					k3 = u32AePointPosValue - fix_point_div_32(AePointPos, fix_point_mult3_32(AePointPos, tw_step_s, k2, k2), (u32AePointPosValue-k2));
					/* ISP_ERROR("tw_step_s %d  k2 %d  k3 %d\n",tw_step_s, k2,k3); */
				} else {
					k3 = u32AePointPosValue - fix_point_mult2_32(AePointPos, tw_step_s, k2);
				}
			}
			if (k3 < u32AePointPosValue + min_step && k3 > u32AePointPosValue)
				k3 = u32AePointPosValue + min_step;
			else if (k3 > u32AePointPosValue - min_step && k3 < u32AePointPosValue)
				k3 = u32AePointPosValue - min_step;
			ev = fix_point_mult2_32(AePointPos, k3, ev_o);
#endif
		}
	}
	/* ISP_ERROR("k2 %d k3 %d ev %d\n", k2,k3,ev); */

    /* ev event */
	ae->_ae_ev = ev;
	/* ISP_ERROR("%s,%d: ae->_ae_ev = 0x%llx\n", __func__, __LINE__, ae->_ae_ev); */
	/* {
	   tisp_event_t ae_event;
	   ae_event.type = TISP_EVENT_TYPE_AE_EV;
	   ae_event.data1 = ae->_ae_ev;
	   tisp_event_push(&ae->core->tisp_event_mg, &ae_event);
	   tisp_ae_ctrls.tisp_ae_ev = ae->_ae_ev;
	   } */

	if ((ev == ev_o) && (ae->trig_deflick == 0) && ae->trig_cal && (0 == ae->tisp_ae_ctrls.tisp_ae_it_manual)){
		exp_t = exp_t_o;
		ag = ag_o;
		dg = dg_o;
	} else {
		ae->trig_cal = 1;
		ae->trig_deflick = 0;
		if (0 == ae->tisp_ae_ctrls.tisp_ae_it_manual) {
			min_ev = fix_point_mult3_32(AePointPos, ae->min_it<<AePointPos, ae->min_ag, ae->min_dg);
			if (ev <= min_ev) {
				exp_t = ae->min_it;
				ag = ae->min_ag;
				dg = ae->min_dg;
			} else {
				extra_ev = fix_point_div_32(AePointPos, ev, min_ev);
				extra_max_it = fix_point_div_32(AePointPos, (uint32_t)max_it<<AePointPos, (uint32_t)ae->min_it<<AePointPos);
				extra_max_ag = fix_point_div_32(AePointPos, max_ag, ae->min_ag);
				extra_max_dg = fix_point_div_32(AePointPos, max_dg, ae->min_dg);
				/*ISP_ERROR("ev %d min_ev %d extra_ev %d extra_max_it %d extra_max_ag %d extra_max_dg %d\n", ev, min_ev, extra_ev, extra_max_it, extra_max_ag, extra_max_dg);*/
				if (deflicker == 0) {
					/* expt --> ag --> dg */
					if (extra_ev <= extra_max_it) {
						exp_t = (uint32_t)(extra_ev >> AePointPos) << AePointPos;
						if (extra_ev >= fix_point_div_32(AePointPos, (uint32_t)alloc_ev_min_th << AePointPos, (uint32_t)ae->min_it << AePointPos)) {
							ag = 1 << AePointPos;
						} else {
							ag = fix_point_div_32(AePointPos, extra_ev, (uint32_t)exp_t);
						}
						dg = 1 << AePointPos;
					} else if (extra_ev <= fix_point_mult2_32(AePointPos, (uint32_t)extra_max_it, extra_max_ag)) {
						exp_t = extra_max_it;
						ag = fix_point_div_32(AePointPos, extra_ev,  (uint32_t)exp_t);
						dg = 1 << AePointPos;
					} else if (extra_ev <= fix_point_mult3_32(AePointPos, (uint32_t)extra_max_it, extra_max_ag, extra_max_dg)) {
						exp_t = extra_max_it;
						ag = extra_max_ag;
						dg = fix_point_div_32(AePointPos, extra_ev, fix_point_mult2_32(AePointPos, (uint32_t)exp_t, extra_max_ag));
					} else {
						exp_t = extra_max_it;
						ag = extra_max_ag;
						dg = extra_max_dg;
						exp_max = 1;
					}
				} else if(deflicker == 1){
					extra_flicker_t = fix_point_div_32(AePointPos, (uint32_t)flicker_t[0] << AePointPos, (uint32_t)ae->min_it << AePointPos);
					if (extra_ev < extra_flicker_t) {
						/*ISP_ERROR("extra_flicker_t %d \n"extra_flicker_t);*/
						if (sub_df == 1) {
							if (fix_point_mult2_32(AePointPos, extra_ev, fix_point_div_32(AePointPos, (uint32_t)(ae_target + sub_df_tol) << AePointPos, (uint32_t)ae_target << AePointPos)) < (uint32_t)extra_flicker_t) {
								exp_t = fix_point_mult2_32(AePointPos, extra_ev, fix_point_div_32(AePointPos, (uint32_t)(ae_target + sub_df_tol) << AePointPos, (uint32_t)ae_target << AePointPos));
								//ev * (ae_target+sub_df_tol)/ae_target;
							} else {
								exp_t = extra_flicker_t;
							}
							ag = 1 << AePointPos;
							dg = 1 << AePointPos;
						} else {
							exp_t = (uint32_t)(extra_ev >> AePointPos) << AePointPos;
							if (((extra_flicker_t - exp_t) <= ((uint32_t)sub_df_tol << AePointPos)) && (ae_stable == 1)){
								exp_t = extra_flicker_t;
							}
							ag = fix_point_div_32(AePointPos, extra_ev, (uint32_t)exp_t);
							dg = 1 << AePointPos;
						}
					} else {
						extra_flicker_t = fix_point_div_32(AePointPos, (uint32_t)flicker_t[max_deflick] << AePointPos, (uint32_t)ae->min_it << AePointPos);
						while (extra_max_it + ((uint32_t)20 << AePointPos) < extra_flicker_t){
							max_deflick--;
							if (max_deflick < 1){
								max_deflick = 1;
								break;
							}
							extra_flicker_t = fix_point_div_32(AePointPos, (uint32_t)flicker_t[max_deflick] << AePointPos, (uint32_t)ae->min_it << AePointPos);
						}
						for(i = 1; i <= max_deflick; i++){
							extra_flicker_t = fix_point_div_32(AePointPos, (uint32_t)flicker_t[i] << AePointPos, (uint32_t)ae->min_it << AePointPos);
							if(extra_ev < extra_flicker_t){
								break;
							}
						}
						/* expt */
						exp_t = fix_point_div_32(AePointPos, (uint32_t)flicker_t[i-1]<<AePointPos, (uint32_t)ae->min_it<<AePointPos);
						if(exp_t >= extra_max_it){
							exp_t = extra_max_it;
						}
						/* ag */
						ag = fix_point_div_32(AePointPos, extra_ev, (uint32_t)exp_t);
						/*ISP_ERROR("extra_ev1 %d  exp_t1 %d  ag1 %d \n", extra_ev, exp_t, ag);*/
						/* dg */
						if (ag > extra_max_ag){
							dg = fix_point_div_32(AePointPos, ag, extra_max_ag);
							/* ISP_ERROR("extra_max_ag %d  ag %d  extra_max_dg %d  dg %d\n", extra_max_ag, ag, extra_max_dg, dg); */
							if (dg > extra_max_dg)
								dg = extra_max_dg;
							ag = extra_max_ag;
							exp_max = 1;
						}else{
							dg = 1 << AePointPos;
						}
					}
				}
				exp_t = (fix_point_mult2_32(AePointPos, (uint32_t)exp_t, (uint32_t)ae->min_it<<AePointPos))>>AePointPos;
				ag = fix_point_mult2_32(AePointPos, ag, ae->min_ag);
				dg = fix_point_mult2_32(AePointPos, dg, ae->min_dg);
				/*ISP_ERROR("exp_t %d  ag %d  dg %d\n", exp_t, ag, dg);*/
			}
		} else if (1 == ae->tisp_ae_ctrls.tisp_ae_it_manual) {
			if (ae->tisp_ae_ctrls.tisp_ae_sensor_integration_time <= ae->min_it)
				exp_t = ae->min_it;
			else if (ae->tisp_ae_ctrls.tisp_ae_sensor_integration_time >= max_it){
				exp_t = max_it;
			}
			else
				exp_t = ae->tisp_ae_ctrls.tisp_ae_sensor_integration_time;
			ae->tisp_ae_ctrls.tisp_ae_sensor_integration_time = exp_t;
			if (ev <= fix_point_mult3_32(AePointPos, (uint32_t)exp_t<<AePointPos, ae->min_ag, ae->min_dg)) {
				ag = ae->min_ag;
				dg = ae->min_dg;
			} else {
				ag = fix_point_div_32(AePointPos, ev, (uint32_t)exp_t << AePointPos);
				if (ag <= fix_point_mult2_32(AePointPos, max_ag, ae->min_dg)) {
					dg = ae->min_dg;
					ag = fix_point_div_32(AePointPos, ag, dg);
				} else {
					dg = fix_point_div_32(AePointPos, ag, max_ag);
					if (dg > max_dg)
						dg = max_dg;
					ag = max_ag;
					exp_max = 1;
				}
			}
		}
	}

	/* de_udexp_num= 1920*1080*20/225; */
	/* de_ovexp_num= 1920*1080*20/225; */
	if (ae_stable == 0) {
		if (ae_stable_old == 1) {
			ae_scene = ae_scene_old;//0;
		} else {
			if (zone_wmean_sm < ae_target) {
				if (udexp_sum > de_udexp_num) {
					if (((uint32_t)scene_roi_wmean << AePointPos) > fix_point_mult2_32(AePointPos, (uint32_t)scene_roui_wmean << AePointPos, flare_up_pct) && scene_roi_wmean > 40) {
						ae_scene = 2;
						//ISP_ERROR("1");
					} else if (fix_point_mult2_32(AePointPos, (uint32_t)scene_roi_wmean << AePointPos, backlight_up_pct) < ((uint32_t)scene_roui_wmean << AePointPos) && scene_roui_wmean > 40) {
						ae_scene = 1;
						//ISP_ERROR("2");
					} else {
						ae_scene = 0;//ae_scene_old;
						//ISP_ERROR("3");
					}
				} else {
					ae_scene = ae_scene_old;
					/* if(ae_scene_old == 2) */
					/* 	ae_scene = 0;//ae_scene_old; */
					/* else */
					/* 	ae_scene = 0; */
					/* ISP_ERROR("4"); */
				}
			} else {
				if (ovexp_sum > de_ovexp_num) {
					if (((uint32_t)scene_roi_wmean<<AePointPos) > fix_point_mult2_32(AePointPos, (uint32_t)scene_roui_wmean<<AePointPos, flare_down_pct)){
						ae_scene = 2;
						//ISP_ERROR("5");
					} else if (fix_point_mult2_32(AePointPos, (uint32_t)scene_roi_wmean<<AePointPos, backlight_down_pct) < ((uint32_t)scene_roui_wmean<<AePointPos)){
						ae_scene = 1;
						//ISP_ERROR("6");
					} else {
						ae_scene = 0;//ae_scene_old;
						//ISP_ERROR("7");
					}
				} else {
					ae_scene = ae_scene_old;
					/* if(ae_scene_old == 1) */
					/* 	ae_scene = ae_scene_old; */
					/* else */
					/* 	ae_scene = 0; */
					/* ISP_ERROR("8"); */
				}
			}
		}
	} else {
		ae_scene = ae_scene_old;
	}
	*(ae_stat + 3) = ae_scene;
	/* ISP_ERROR(" %d  %d %d  %d\n",ae_scene, zone_wmean, zone_wmean_sm, ae_target); */

	if (exp_max == 1) {
		if (exp_max_cnt < 255) {
			exp_max_cnt = exp_max_cnt + 1;
		}
	} else {
		exp_max_cnt = 0;
	}
	if (exp_max_cnt == max_exp_cnt_th)
		ae_stable = 1;

	*(ae_stat + 1) = ae_stable;
	*(ae_stat + 4) = exp_max_cnt;

#if 0
	//for ov2735 analog gain
	if (ag > 130023424) {  //7.75*2^24
		ag_r_h = 8;
		ag_h = 8;
		if (ag >= 8*16777216) {
			ag_r_l = (fix_point_mult2(AePointPos, fix_point_div(AePointPos, ag, (uint64_t)8 << AePointPos) - ((uint64_t)1 << AePointPos), (uint64_t)16 << AePointPos) + 8388608) >> AePointPos;
			//(int)((ag/8 - 1) * 16 + 0.5);
		} else {
			ag_r_l = (8388608 - fix_point_mult2(AePointPos, ((uint64_t)1 << AePointPos) - fix_point_div(AePointPos, ag, (uint64_t)8 << AePointPos), (uint64_t)16 << AePointPos)) >> AePointPos;
		}
	} else if (ag<=32505856) {  //1.9375*2^24
		ag_r_h = 1;
		ag_h = 1;
		if (ag >= 1 * 16777216) {
			ag_r_l = (fix_point_mult2(AePointPos, fix_point_div(AePointPos, ag, (uint64_t)1 << AePointPos) - ((uint64_t)1 << AePointPos), (uint64_t)16 << AePointPos) + 8388608) >> AePointPos;
			//(int)((ag/1 - 1) * 16 + 0.5);
		} else {
			ag_r_l=(8388608 - fix_point_mult2(AePointPos, ((uint64_t)1 << AePointPos) - fix_point_div(AePointPos, ag, (uint64_t)1 << AePointPos), (uint64_t)16 << AePointPos)) >> AePointPos;
		}
	} else if (ag > 32505856 && ag <= 65011712) {  //1.9375*2^24  3.875*2^24
		ag_r_h = 2;
		ag_h = 2;
		if (ag >= 2 * 16777216) {
			ag_r_l = (fix_point_mult2(AePointPos, fix_point_div(AePointPos, ag, (uint64_t)2 << AePointPos) - ((uint64_t)1 << AePointPos), (uint64_t)16 << AePointPos) + 8388608) >> AePointPos;
			//(int)((ag/2 - 1) * 16 + 0.5);
		} else {
			ag_r_l = (8388608 - fix_point_mult2(AePointPos, ((uint64_t)1 << AePointPos) - fix_point_div(AePointPos, ag, (uint64_t)2 << AePointPos), (uint64_t)16 << AePointPos)) >> AePointPos;
		}
	} else if (ag > 65011712 && ag <= 130023424) {  //3.875*2^24  7.75*2^24
		ag_r_h = 4;
		ag_h = 4;
		if (ag >= 4 * 16777216) {
			ag_r_l = (fix_point_mult2(AePointPos, fix_point_div(AePointPos, ag, (uint64_t)4 << AePointPos) - ((uint64_t)1 << AePointPos), (uint64_t)16 << AePointPos) + 8388608) >> AePointPos;
			/* (int)((ag/4 - 1) * 16 + 0.5); */
		} else {
			ag_r_l = (8388608 - fix_point_mult2(AePointPos, ((uint64_t)1 << AePointPos) - fix_point_div(AePointPos, ag, (uint64_t)4 << AePointPos), (uint64_t)16 << AePointPos)) >> AePointPos;
		}
	}
	/* ISP_ERROR("ag %llu ag_r_l %d\n", ag, ag_r_l); */

	ag_r = ag_r_h * (16 + ag_r_l);
	ae->ag_new = fix_point_mult2(AePointPos, (uint64_t)ag_h << AePointPos,
			((uint64_t)1 << AePointPos) +
			fix_point_div(AePointPos,
				(uint64_t)ag_r_l << AePointPos,
				(uint64_t)16 << AePointPos));
	/* (float)ag_h * (1 + (float)ag_r_l / 16); */
#endif
#if 0
	if ((ae->ftune == 1) || (ag != ag_o) || (dg != dg_o) || tisp_ae_ctrls.tisp_ae_manual){
		if (tisp_ae_ctrls.tisp_ae_manual) {
			ag = tisp_ae_ctrls.tisp_ae_sensor_again;
			dg = fix_point_mult2_32(AePointPos,
					tisp_ae_ctrls.tisp_ae_sensor_dgain,
					tisp_ae_ctrls.tisp_ae_isp_dgian);
		}

		ae->ag_new = tisp_set_sensor_analog_gain(ag);
		tisp_ae_ctrls.tisp_ae_sensor_again = ae->ag_new;
		/* ISP_ERROR("%s,%d: want ag = %d, get ag = %d\n", __func__, __LINE__, ag, ae->ag_new); */

		ae->dg_new = tisp_set_sensor_digital_gain(dg);
		tisp_ae_ctrls.tisp_ae_sensor_dgain = ae->dg_new;
		/* ISP_ERROR("%s,%d: want dg = %d, get dg = %d\n", __func__, __LINE__, dg, ae->dg_new); */

		ae->dg_new = fix_point_div_32(AePointPos, fix_point_mult2_32(AePointPos, ag, dg),fix_point_mult2_32(AePointPos, ae->ag_new, ae->dg_new));
		if (ae->dg_new > max_dg)
			ae->dg_new = max_dg;
		tisp_ae_ctrls.tisp_ae_isp_dgian = ae->dg_new;
		/* ISP_ERROR("%s,%d: want dg = %d, get dg = %d\n", __func__, __LINE__, dg, ae->dg_new); */
		ae->ftune = 0;
	}
#endif
	*(ae_reg + 0) = ag_r;    // unsigned long
	*(ae_reg + 1) = dg;      // float -->uint32_t  ae->dg_new
	*(ae_reg + 2) = exp_t;   // unsigned long
	*(ae_reg + 3) = ag;      // float -->uint32_t   ae->ag_new
	/* if(ae->dg_new < ((uint32_t)1<<AePointPos)) */
	/* *(ae_reg + 1) = ((uint32_t)1<<AePointPos); */
	/*ISP_ERROR("ev %d  expt %d  ag %d  ae->ag_new %d  dg %d  ae->dg_new %d\n", ev>>10, exp_t, ag, ae->ag_new, dg, ae->dg_new); */
}

int tisp_ae_g_min(tisp_ae_t *ae, tisp_ae_ex_min_t *ae_min)
{
	ae_min->min_it = ae->min_it;
	ae_min->min_again = ae->min_ag;

	return 0;
}

int tisp_ae_s_min(tisp_ae_t *ae, tisp_ae_ex_min_t ae_min)
{
	if ((ae_min.min_it <= 0) || (ae_min.min_again < 1024)) {
		ISP_ERROR("invaild min integration or min sensor analog gain\n");
		return -1;
	} else {
		ae->min_it = ae_min.min_it;
		ae->min_ag = ae_min.min_again;
	}

	ae->trig_cal = 0;

	return 0;
}

void Tiziano_ae_fpga(tisp_ae_t *ae,
		int *ae_stat, int *ae_hist_array,
		int *scene_para)
{
	int zone_wmean = 0;
	int scene_roi_wmean;
	int scene_roui_wmean;
	int de_ovexp_en = scene_para[0];
	int de_ovexp_th = scene_para[1];
	int de_ovexp_num = scene_para[2];
	int de_udexp_th = scene_para[3];
	int de_udexp_num = scene_para[4];
	int de_flare_dark_w = scene_para[5];
	int de_flare_sat_w = scene_para[6];
	int de_bl_dark_w = scene_para[7];
	int de_bl_sat_w = scene_para[8];
	int dark_weight = scene_para[9];
	int sat_weight= scene_para[10];
	int ovexp_sum = 0;
	int udexp_sum = 0;
	int dark_weight_scene = 0;
	int sat_weight_scene = 0;
	int ae_scene = *(ae_stat + 3);
	int i;
	int pic_ysum = 0;
	uint32_t th_pct[2] = {0, 0};

	if (ae_scene == 0){
		dark_weight_scene = dark_weight;
		sat_weight_scene = sat_weight;
	} else if (ae_scene == 1) {
		dark_weight_scene = de_bl_dark_w;
		sat_weight_scene = de_bl_sat_w;
	} else if (ae_scene == 2) {
		dark_weight_scene = de_flare_dark_w;
		sat_weight_scene = de_flare_sat_w;
	}

	ae_weight_mean2(ae, dark_weight_scene,
			sat_weight_scene, &zone_wmean, th_pct,
			&pic_ysum, &scene_roi_wmean, &scene_roui_wmean);

	if (de_ovexp_en == 1) {
		for (i = de_ovexp_th; i < 256; i++) {
			ovexp_sum = ovexp_sum + ae->ae_hist_array[i];
		}
		for (i = 0; i < de_udexp_th; i++) {
			udexp_sum = udexp_sum + ae->ae_hist_array[i];
		}
	} else {
		ovexp_sum = 0;
		udexp_sum = 0;
	}

	ae_tune2(ae, zone_wmean, th_pct, ovexp_sum,
		 de_ovexp_num, udexp_sum, de_udexp_num,
		 scene_roi_wmean, scene_roui_wmean, scene_para);
}


void tisp_set_ag_func(tisp_ae_t *ae){

    uint32_t *ae_result = ae->IspAeTuneParam.a_ae_result;
	uint32_t *ae_reg = ae->IspAeTuneParam.a_ae_reg;

    uint32_t ag_o = ae_result[3];
	uint32_t dg_o = ae_result[1];

    uint32_t ag = *(ae_reg + 3);
	uint32_t dg = *(ae_reg + 1);

    int *exp_parameter = ae->IspAeTuneParam.a_exp_parameter;
	uint32_t max_dg = (uint32_t)exp_parameter[4];

	uint32_t AePointPos = ae->IspAeTuneParam.a_AePointPos[0];

	if ((ae->ftune == 1) || (ag != ag_o) || (dg != dg_o) || ae->tisp_ae_ctrls.tisp_ae_manual){
		if (ae->tisp_ae_ctrls.tisp_ae_manual) {
			ag = ae->tisp_ae_ctrls.tisp_ae_sensor_again;
			dg = fix_point_mult2_32(AePointPos,
						ae->tisp_ae_ctrls.tisp_ae_sensor_dgain,
						ae->tisp_ae_ctrls.tisp_ae_isp_dgian);
		}

		ae->ag_new = tisp_set_sensor_analog_gain(ae, ag);
		ae->tisp_ae_ctrls.tisp_ae_sensor_again = ae->ag_new;
		/* ISP_ERROR("%s,%d: want ag = %d, get ag = %d\n", __func__, __LINE__, ag, ae->ag_new); */

		ae->dg_new = tisp_set_sensor_digital_gain(ae, dg);
		ae->tisp_ae_ctrls.tisp_ae_sensor_dgain = ae->dg_new;
		//ISP_ERROR("%s,%d: want dg = %d, get dg = %d\n", __func__, __LINE__, dg, ae->dg_new);

		ae->dg_new = fix_point_div_32(AePointPos, fix_point_mult2_32(AePointPos, ag, dg),fix_point_mult2_32(AePointPos, ae->ag_new, ae->dg_new));
		if (ae->dg_new > max_dg)
			ae->dg_new = max_dg;
		ae->tisp_ae_ctrls.tisp_ae_isp_dgian = ae->dg_new;
		/* ISP_ERROR("%s,%d: want dg = %d, get dg = %d\n", __func__, __LINE__, dg, ae->dg_new); */
		ae->ftune = 0;
	}
	*(ae_reg + 1) = ae->dg_new;  // float -->uint32_t
	*(ae_reg + 3) = ae->ag_new; // float -->uint32_t
	//ISP_ERROR("expt %d  ag %d  ae->ag_new %d  dg %d  ae->dg_new %d\n", *(ae_reg + 2), ag, *(ae_reg + 3), dg, *(ae_reg + 1));
}

static uint32_t ev_cache[10] = {1024,1024,1024,1024,1024,1024,1024,1024,1024};
static uint32_t ad_cache[10] = {1024,1024,1024,1024,1024,1024,1024,1024,1024};
static uint32_t ag_cache[9]  = {1024,1024,1024,1024,1024,1024,1024,1024,1024};
static uint32_t dg_cache[9]  = {1024,1024,1024,1024,1024,1024,1024,1024,1024};
int tisp_ae_process_impl(tisp_ae_t *ae)
{
	uint32_t AePointPos = ae->_AePointPos[0];
	unsigned int _dg_reg[2] = {1024 << 16 | 1024, 1024 << 16 | 1024};
	uint32_t no_wb_static[2] = {1 << AePointPos,1 << AePointPos};
	uint32_t ae_integation_time = 0;
	tisp_core_t *core = ae->core;

	/* IspAeWmeanParam */
	ae->IspAeWmeanParam.a_ae_array_d = ae->ae_array_d;
	ae->IspAeWmeanParam.a_ae_array_m = ae->ae_array_m;
	ae->IspAeWmeanParam.a_ae_array_s = ae->ae_array_s;
	ae->IspAeWmeanParam.a_ae_array_ir = ae->ae_array_ir;
	ae->IspAeWmeanParam.a_ae_array_dc = ae->ae_array_dc;
	ae->IspAeWmeanParam.a_ae_array_sc = ae->ae_array_sc;
	ae->IspAeWmeanParam.a_ae_parameter = ae->_ae_parameter;
	ae->IspAeWmeanParam.a_ae_zone_weight = ae->_ae_zone_weight;
	ae->IspAeWmeanParam.a_exp_parameter = ae->_exp_parameter;
	ae->IspAeWmeanParam.a_ae_stat = ae->_ae_stat;
	ae->IspAeWmeanParam.a_scene_roui_weight= ae->_scene_roui_weight;
	ae->IspAeWmeanParam.a_scene_roi_weight = ae->_scene_roi_weight;
	ae->IspAeWmeanParam.a_log2_lut = ae->_log2_lut;
	ae->IspAeWmeanParam.a_weight_lut = ae->_weight_lut;
	ae->IspAeWmeanParam.a_AePointPos = ae->_AePointPos;

	/* IspAeTuneParam */
	ae->IspAeTuneParam.a_exp_parameter = ae->_exp_parameter;
	ae->IspAeTuneParam.a_ev_list = ae->_ev_list;
	ae->IspAeTuneParam.a_lum_list = ae->_lum_list;
	ae->IspAeTuneParam.a_at_list = ae->_at_list;
	ae->IspAeTuneParam.a_ae_result = ae->_ae_result;
	ae->IspAeTuneParam.a_ae_reg = ae->_ae_reg;
	ae->IspAeTuneParam.a_ae_stat = ae->_ae_stat;
	ae->IspAeTuneParam.a_ae_wm_q = ae->_ae_wm_q;
	ae->IspAeTuneParam.a_flicker_t = ae->_deflick_lut;
	ae->IspAeTuneParam.a_deflicker_para = ae->_deflicker_para;
	ae->IspAeTuneParam.a_ae_ev_step = ae->ae_ev_step;
	ae->IspAeTuneParam.a_ae_scene_mode_th = ae->ae_scene_mode_th;
	ae->IspAeTuneParam.a_ae_stable_tol = ae->ae_stable_tol;
	ae->IspAeTuneParam.a_AePointPos = ae->_AePointPos;
	ae->IspAeTuneParam.a_ae_hist_ir_array = ae->ae_hist_ir_array;
	ae->IspAeTuneParam.a_ae_nodes_num = &ae->_nodes_num;
	ae->IspAeTuneParam.a_ae_compensation = &ae->ae_compensation;

#if 0
	if (EffectCount == 0) {
		/* ae algorithm */
		Tiziano_ae_fpga(IspAeWmeanParam, ae->IspAeTuneParam, _ae_stat, ae->ae_hist_array, _scene_para);
		/* set ag */
		tisp_set_ag_func(ae);
		/* update result */
        memcpy(_ae_result, ae->_ae_reg, sizeof(ae->_ae_reg));
        /* set expt */
		ae_integation_time = ae->_ae_reg[2]; // exp_t
		if (tisp_set_sensor_integration_time(ae, ae_integation_time)) {
			ISP_ERROR("sorry,set integration time failed!\n");
			return -1;
		}
	}
	if (ae->EffectCount == ae->_exp_parameter[10]) {
		/* set dg */
		JZ_Isp_Ae_Dg2reg(AePointPos, _dg_reg, ae->_ae_reg[1], no_wb_static); //de_new
		system_reg_write_ae(2, GIB_ADDR_R_G, _dg_reg[0]);
		system_reg_write_ae(2, GIB_ADDR_B_IR, _dg_reg[1]);

		/* ev event */
		{
			tisp_event_t ae_event;
			ae_event.type = TISP_EVENT_TYPE_AE_EV;
			ae_event.data1 = ae->_ae_ev;
			tisp_event_push(&core->tisp_event_mg, &ae_event);
			tisp_ae_ctrls.tisp_ae_ev = ae->_ae_ev;
		}

		/* ag event */
		/* --> Demosaic */
		ae->total_gain_new = (uint32_t)(fix_point_mult2_32(AePointPos, ae->_ae_reg[1], _ae->ae_reg[3]) << (16 - 10));
		if ((ae->total_gain_old != ae->total_gain_new) || tisp_ae_ctrls.tisp_ae_manual) {
			uint32_t tgain_db = 0;
			tisp_event_t tgain_event;
			ae->total_gain_old = ae->total_gain_new;
			tgain_db = tisp_log2_fixed_to_fixed(ae->total_gain_new, 16, 16);
			/* ISP_ERROR("%s,%d: ae->total_gain_new = 0x%x, tgain_db = 0x%x\n", __func__, __LINE__, ae->total_gain_new, tgain_db); */
			tgain_event.type = TISP_EVENT_TYPE_AE_TGAIN_UPDATE;
			tgain_event.data1 = tgain_db;
			tisp_event_push(&core->tisp_event_mg, &tgain_event);
			tisp_ae_ctrls.tisp_ae_tgain_db = tgain_db;
		}

		/* agdg event */
		/* --> BLC */
		ae->again_new = (uint32_t)(ae->_ae_reg[3] << (16 - 10));
		if ((ae->again_old != ae->again_new) || tisp_ae_ctrls.tisp_ae_manual) {
			uint32_t again_db = 0;
			tisp_event_t again_event;
			ae->again_old = ae->again_new;
			again_db = tisp_log2_fixed_to_fixed(ae->again_new, 16, 16);
			/* ISP_ERROR("%s,%d: ae->total_gain_new = 0x%x, tgain_db = 0x%x\n",
			   __func__, __LINE__, ae->total_gain_new, tgain_db); */
			again_event.type = TISP_EVENT_TYPE_AE_AGAIN_UPDATE;
			again_event.data1 = again_db;
			tisp_event_push(&core->tisp_event_mg, &again_event);
			tisp_ae_ctrls.tisp_ae_again_db = again_db;
		}
	}

	ae->EffectCount ++;
	if (ae->EffectCount >= (ae->_exp_parameter[10]+1)) {
		ae->EffectCount = 0;
	}
#else
	/* ae algorithm */
	Tiziano_ae_fpga(ae, ae->_ae_stat, ae->ae_hist_array, ae->_scene_para);
	/* set ag */
	tisp_set_ag_func(ae);
    /* set expt */
	ae_integation_time = ae->_ae_reg[2]; // exp_t
	if (tisp_set_sensor_integration_time(ae, ae_integation_time)) {
		ISP_ERROR("sorry,set integration time failed!\n");
		return -1;
	}
	/* Effect Cache ev total_gain*/
	ae->EffectFrame = ae->_exp_parameter[10];
	ae->EffectCount = ae->EffectFrame;
	while(ae->EffectCount+1>0){
		ae->ev_cache[ae->EffectCount+1] = ae->ev_cache[ae->EffectCount];
		ae->ad_cache[ae->EffectCount+1] = ae->ad_cache[ae->EffectCount];
		ae->EffectCount--;
	}
	ae->ev_cache[0] = fix_point_mult3_32(AePointPos, (uint32_t)ae->_ae_reg[2]<<AePointPos, ae->_ae_reg[3], ae->_ae_reg[1]);  //--> expt ag dg
	ae->ad_cache[0] = fix_point_mult2_32(AePointPos,                                   ae->_ae_reg[3],ae->_ae_reg[1]);  //-->      ag dg
	/* Effect Cache ag dg */
	ae->EffectCount = ae->EffectFrame;
	while(ae->EffectCount>0){
		ae->ag_cache[ae->EffectCount] = ae->ag_cache[ae->EffectCount-1]; // --> push
		ae->dg_cache[ae->EffectCount] = ae->dg_cache[ae->EffectCount-1]; // --> set sensor
		ae->EffectCount--;
	}
	ae->ag_cache[0] = ae->_ae_reg[3]; // --> push
	ae->dg_cache[0] = ae->_ae_reg[1]; // --> set sensor

	/* update result */
    memcpy(ae->_ae_result, ae->_ae_reg, sizeof(ae->_ae_reg));

	/* set dg */
	JZ_Isp_Ae_Dg2reg(AePointPos, _dg_reg, ae->dg_cache[ae->EffectFrame], no_wb_static); //de_new
	system_reg_write_ae(ae, 2, GIB_ADDR_R_G, _dg_reg[0]);
	system_reg_write_ae(ae, 2, GIB_ADDR_B_IR, _dg_reg[1]);

	/* ev event */
	{
		tisp_event_t ae_event;
		ae_event.type = TISP_EVENT_TYPE_AE_EV;
		ae_event.data1 = ae->ev_cache[ae->EffectFrame+1];
		tisp_event_push(&core->tisp_event_mg, &ae_event);
		ae->tisp_ae_ctrls.tisp_ae_ev = ae_event.data1;
	}

	/* ag event */
	/* --> Demosaic */
	ae->total_gain_new = (uint32_t)(ae->ad_cache[ae->EffectFrame+1] << (16 - 10));
	if ((ae->total_gain_old != ae->total_gain_new) || ae->tisp_ae_ctrls.tisp_ae_manual) {
		uint32_t tgain_db = 0;
		tisp_event_t tgain_event;
		ae->total_gain_old = ae->total_gain_new;
		tgain_db = tisp_log2_fixed_to_fixed(ae->total_gain_new, 16, 16);
		/* ISP_ERROR("%s,%d: ae->total_gain_new = 0x%x, tgain_db = 0x%x\n", __func__, __LINE__, ae->total_gain_new, tgain_db); */
		tgain_event.type = TISP_EVENT_TYPE_AE_TGAIN_UPDATE;
		tgain_event.data1 = tgain_db;
		tisp_event_push(&core->tisp_event_mg, &tgain_event);
		ae->tisp_ae_ctrls.tisp_ae_tgain_db = tgain_db;
	}

	/* agdg event */
	/* --> BLC */
	ae->again_new = (uint32_t)(ae->ag_cache[ae->EffectFrame] << (16 - 10));
	if ((ae->again_old != ae->again_new) || ae->tisp_ae_ctrls.tisp_ae_manual) {
		uint32_t again_db = 0;
		tisp_event_t again_event;
		ae->again_old = ae->again_new;
		again_db = tisp_log2_fixed_to_fixed(ae->again_new, 16, 16);
		again_event.type = TISP_EVENT_TYPE_AE_AGAIN_UPDATE;
		again_event.data1 = again_db;
		tisp_event_push(&core->tisp_event_mg, &again_event);
		ae->tisp_ae_ctrls.tisp_ae_again_db = again_db;
	}
#endif

	return 0;
}

int32_t tisp_ae_ctrls_update(tisp_ae_t *ae)
{
	uint32_t again;
	tisp_core_t *core = ae->core;

	if (ae->tisp_ae_ctrls.tisp_ae_max_sensor_integration_time <= core->sensor_ctrl.param.integration_time_max) {
		ae->_exp_parameter[2] = ae->tisp_ae_ctrls.tisp_ae_max_sensor_integration_time;
	} else {
		ae->tisp_ae_ctrls.tisp_ae_max_sensor_integration_time = ae->_exp_parameter[2];
	}

	again = tisp_math_exp2(core->sensor_ctrl.param.again_log2_max, 16, 10);
	//ISP_ERROR("%s:%d:::again is %d,max sensor again is %d\n",__func__,__LINE__,again,tisp_ae_ctrls.tisp_ae_max_sensor_again);
	if (ae->tisp_ae_ctrls.tisp_ae_max_sensor_again <= again) {
		ae->_exp_parameter[3] = ae->tisp_ae_ctrls.tisp_ae_max_sensor_again;
	} else {
		ae->tisp_ae_ctrls.tisp_ae_max_sensor_again = ae->_exp_parameter[3];
	}
	//ISP_ERROR("%s:%d:::exp para is %d,again is %d,max sensor again is %d\n",__func__,__LINE__,ae->_exp_parameter[3],again,tisp_ae_ctrls.tisp_ae_max_sensor_again);

	if (ae->tisp_ae_ctrls.tisp_ae_max_isp_dgain != ae->_exp_parameter[4]) {
		ae->_exp_parameter[4] = ae->tisp_ae_ctrls.tisp_ae_max_isp_dgain;
	}

	return 0;
}

int ae_interrupt_static(void *data)
{
	int num = 0;
	tisp_ae_t *ae = (tisp_ae_t *)data;
	tisp_core_t *core = ae->core;
	tisp_info_t *tispinfo = &core->tispinfo;


	num = (system_reg_read(((tisp_core_t*)ae->core)->priv_data, AE_STAT_ADDR_DMA_INFO) >> 4) & 0x3;
	dma_cache_sync(NULL, (void*)tispinfo->buf_ae_vaddr+num * 4 * 1024, 4096, DMA_BIDIRECTIONAL);
	tisp_ae_get_statistics(ae, (void*)(tispinfo->buf_ae_vaddr + num * 4 * 1024), 0xf001f001);
	/* ISP_ERROR_func(); */
	/* ISP_ERROR(" \n"); */

	if(ae->trig == 1){
		ae->_ae_stat[1] = 0;
		ae->trig = 0;
	}

	return IRQ_HANDLED;
}

int tisp_ae_process(void *data, uint64_t data1, uint64_t data2, uint64_t data3, uint64_t data4)
{
	tisp_ae_t *ae = data;
	tisp_ae_ctrls_update(ae);
	tisp_ae_process_impl(ae);

	return 0;
}

int ae_interrupt_hist(void *data)
{
	int num = 0;
	tisp_ae_t *ae = (tisp_ae_t *)data;
	tisp_core_t *core = ae->core;
	tisp_info_t *tispinfo = &core->tispinfo;

	num = system_reg_read(((tisp_core_t*)ae->core)->priv_data, AE_STAT_ADDR_DMA_INFO) & 0x3;
	dma_cache_sync(NULL, (void*)tispinfo->buf_aehist_vaddr +
		       num * 2 * 1024, 2048, DMA_BIDIRECTIONAL);
	tisp_ae_get_hist(ae, (void*)(tispinfo->buf_aehist_vaddr + num * 2 * 1024), 1, 1);

	/* ISP_ERROR_func(); */
#ifdef TISP_AE_PROCESS_THREAD
	{
		tisp_event_t ae_event;
		ae_event.type = TISP_EVENT_TYPE_AE_PROCESS;
		tisp_event_push(&core->tisp_event_mg, &ae_event);
	}
#else
	tisp_ae_process(ae, 0, 0, 0, 0);
#endif

	return IRQ_WAKE_THREAD;
}

#define DUMP_AE_REG(ae, name) ISP_ERROR("0x%x: 0x%xx\n", name, system_reg_read(((tisp_core_t*)ae->core)->priv_data, name));

void tiziano_ae_dump(tisp_ae_t *ae)
{
	ISP_ERROR("-----ae regs dump-----\n");
	DUMP_AE_REG(ae, AE_STAT_ADDR_ZONE_NUM_START);
	DUMP_AE_REG(ae, AE_STAT_ADDR_HOR_ZONE_SIZE_14);
	DUMP_AE_REG(ae, AE_STAT_ADDR_HOR_ZONE_SIZE_58);
	DUMP_AE_REG(ae, AE_STAT_ADDR_HOR_ZONE_SIZE_912);
	DUMP_AE_REG(ae, AE_STAT_ADDR_HOR_ZONE_SIZE_1315);
	DUMP_AE_REG(ae, AE_STAT_ADDR_VER_ZONE_SIZE_14);
	DUMP_AE_REG(ae, AE_STAT_ADDR_VER_ZONE_SIZE_58);
	DUMP_AE_REG(ae, AE_STAT_ADDR_VER_ZONE_SIZE_912);
	DUMP_AE_REG(ae, AE_STAT_ADDR_VER_ZONE_SIZE_1315);
	DUMP_AE_REG(ae, AE_STAT_ADDR_LUM_TH_FREQ);
	DUMP_AE_REG(ae, AE_STAT_ADDR_AE_DMA_BASE_1);
	DUMP_AE_REG(ae, AE_STAT_ADDR_AE_DMA_BASE_2);
	DUMP_AE_REG(ae, AE_STAT_ADDR_AE_DMA_BASE_3);
	DUMP_AE_REG(ae, AE_STAT_ADDR_AE_DMA_BASE_4);
	DUMP_AE_REG(ae, AE_STAT_ADDR_HIST_DMA_BASE_1);
	DUMP_AE_REG(ae, AE_STAT_ADDR_HIST_DMA_BASE_2);
	DUMP_AE_REG(ae, AE_STAT_ADDR_HIST_DMA_BASE_3);
	DUMP_AE_REG(ae, AE_STAT_ADDR_HIST_DMA_BASE_4);
	DUMP_AE_REG(ae, AE_STAT_ADDR_DMA_BASE_NUM);
	DUMP_AE_REG(ae, AE_STAT_ADDR_DMA_INFO);
	DUMP_AE_REG(ae, AE_STAT_ADDR_DEBUG0);
	DUMP_AE_REG(ae, AE_STAT_ADDR_DEBUG1);
}

int tiziano_ae_params_refresh(tisp_ae_t *ae)
{
	memcpy(ae->_ae_parameter, tparams.params_data.TISP_PARAM_AE_PARAMETER,
	       sizeof(tparams.params_data.TISP_PARAM_AE_PARAMETER));
	memcpy(ae->ae_switch_night_mode, tparams.params_data.TISP_PARAM_AE_SWITCH_NIGHT_MODE,
	       sizeof(tparams.params_data.TISP_PARAM_AE_SWITCH_NIGHT_MODE));
	memcpy(ae->_AePointPos, tparams.params_data.TISP_PARAM_AE_POINTPOS,
	       sizeof(tparams.params_data.TISP_PARAM_AE_POINTPOS));
	memcpy(ae->_exp_parameter, tparams.params_data.TISP_PARAM_AE_EXP_PARAMETER,
	       sizeof(tparams.params_data.TISP_PARAM_AE_EXP_PARAMETER));
	memcpy(ae->ae_ev_step, tparams.params_data.TISP_PARAM_AE_EV_STEP,
	       sizeof(tparams.params_data.TISP_PARAM_AE_EV_STEP));
	memcpy(ae->ae_stable_tol, tparams.params_data.TISP_PARAM_AE_STABLE_TOL,
	       sizeof(tparams.params_data.TISP_PARAM_AE_STABLE_TOL));
	memcpy(ae->_ev_list, tparams.params_data.TISP_PARAM_AE_EV_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_AE_EV_LIST));
	memcpy(ae->_lum_list, tparams.params_data.TISP_PARAM_AE_LUM_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_AE_LUM_LIST));
	memcpy(ae->_at_list, tparams.params_data.TISP_PARAM_AE_AT_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_AE_AT_LIST));
	memcpy(ae->_deflicker_para, tparams.params_data.TISP_PARAM_AE_DEFLICKER_PARA,
	       sizeof(tparams.params_data.TISP_PARAM_AE_DEFLICKER_PARA));
	memcpy(ae->_flicker_t, tparams.params_data.TISP_PARAM_AE_FLICKER_T,
	       sizeof(tparams.params_data.TISP_PARAM_AE_FLICKER_T));
	memcpy(ae->_scene_para, tparams.params_data.TISP_PARAM_AE_SCENE_PARE,
	       sizeof(tparams.params_data.TISP_PARAM_AE_SCENE_PARE));
	memcpy(ae->ae_scene_mode_th, tparams.params_data.TISP_PARAM_AE_SCENE_MODE_TH,
	       sizeof(tparams.params_data.TISP_PARAM_AE_SCENE_MODE_TH));
	memcpy(ae->_log2_lut, tparams.params_data.TISP_PARAM_AE_ROI_TREND_LOG2_LUT,
	       sizeof(tparams.params_data.TISP_PARAM_AE_ROI_TREND_LOG2_LUT));
	memcpy(ae->_weight_lut, tparams.params_data.TISP_PARAM_AE_ROI_TREND_WEIGHT_LUT,
	       sizeof(tparams.params_data.TISP_PARAM_AE_ROI_TREND_WEIGHT_LUT));
	memcpy(ae->_ae_zone_weight, tparams.params_data.TISP_PARAM_AE_ZONE_WEIGHT,
	       sizeof(tparams.params_data.TISP_PARAM_AE_ZONE_WEIGHT));
	memcpy(ae->_scene_roui_weight, tparams.params_data.TISP_PARAM_AE_SCENE_ROUI_WEIGHT,
	       sizeof(tparams.params_data.TISP_PARAM_AE_SCENE_ROUI_WEIGHT));
	memcpy(ae->_scene_roi_weight, tparams.params_data.TISP_PARAM_AE_SCENE_ROI_WEIGHT,
	       sizeof(tparams.params_data.TISP_PARAM_AE_SCENE_ROI_WEIGHT));

	if(ae->ae_dn_refresh_flag == 0){
		memcpy(ae->_ae_result, tparams.params_data.TISP_PARAM_AE_RESULT,
		       sizeof(tparams.params_data.TISP_PARAM_AE_RESULT));
		memcpy(ae->_ae_stat, tparams.params_data.TISP_PARAM_AE_STAT,
		       sizeof(tparams.params_data.TISP_PARAM_AE_STAT));
		memcpy(ae->_ae_wm_q, tparams.params_data.TISP_PARAM_AE_WM_Q,
		       sizeof(tparams.params_data.TISP_PARAM_AE_WM_Q));
	}
	ae->ae_dn_refresh_flag = 0;

	return 0;
}

int tiziano_ae_dn_params_refresh(tisp_ae_t *ae)
{
	ae->ftune = 1;

	ae->ae_dn_refresh_flag = 1;
	tiziano_ae_params_refresh(ae);
	ae->tisp_ae_ctrls.tisp_ae_max_isp_dgain = ae->_exp_parameter[4];

	return 0;
}

int tiziano_ae_compensation_set(tisp_ae_t *ae, uint32_t data)
{
	ae->ae_compensation = data;
	ae->trig = 1;
	ae->force_trig = 1;
	/*ISP_ERROR("ae_compensation %d\n",ae->ae_compensation);*/

	return 0;
}

void tiziano_ae_s_ev_start(tisp_ae_t *ae, unsigned int ev_start)
{
	ae->ae_ev_init_strict = ev_start;
	ae->ae_ev_init_en = 1;
}

int tiziano_ae_s_max_again(tisp_ae_t *ae, unsigned int max_again)
{
	tisp_core_t *core = ae->core;
	if ((max_again < 0) || (max_again > (core->sensor_ctrl.param.again_log2_max<<11))){
		ISP_ERROR("%d not in range, max_again must between 0~%d\n",max_again,core->sensor_ctrl.param.again_log2_max<<11);
		return -1;
	}
	ae->tisp_ae_ctrls.tisp_ae_max_sensor_again = tisp_math_exp2(max_again, 5, 10);


	return 0;
}

int tiziano_ae_s_max_isp_dgain(tisp_ae_t *ae, unsigned int max_isp_dgain)
{
	if (max_isp_dgain < 0){
		ISP_ERROR("%d not in range, max_isp_dgain must > 0\n",max_isp_dgain);
		return -1;
	}
	ae->tisp_ae_ctrls.tisp_ae_max_isp_dgain = tisp_math_exp2(max_isp_dgain, 5, 10);

	return 0;
}

void tiziano_deflicker_expt(tisp_ae_t *ae, uint32_t pal, uint32_t fps,
			    uint32_t vts, uint32_t hts,
			    uint32_t *deflick_lut, uint32_t *nodes_num)
{
	int i;
	uint32_t fps_num;
	ae->_flicker_t[0] = pal;
	ae->_flicker_t[1] = fps;
	ae->_flicker_t[2] = vts;
	ae->_flicker_t[3] = hts;

	fps_num = fix_point_div_32(16, (fps >> 16) <<16, (fps & 0xffff) << 16);
	*nodes_num = fix_point_div_32(16, (uint32_t)(pal*2)<<16, fps_num) >> 16;

	/* vts/expt_pal; */
	/* pal*2/fps; */
	if (*nodes_num > 120)
		*nodes_num = 120;
	else if (*nodes_num < 1)
		*nodes_num = 1;

	for (i = 1; i <= *nodes_num; i++) {
		deflick_lut[i - 1] = (fix_point_mult3_32(16, (uint32_t)i << 16, fps_num, fix_point_div_32(16, (uint32_t)vts << 16, (uint32_t)(pal * 2) << 16)) + 32768) >> 16;
		/* i * expt_pal; */
		/*     expt_pal = interval / linetime; */
		/* 	       interval = 1 / (pal * 2); */
		/* 	       linetime = 1 / vts / fps; */
		/* i * vts * fps / (pal*2); */
	}
	for (i = *nodes_num; i < 120; i++)
		deflick_lut[i] = deflick_lut[*nodes_num - 1];

	*nodes_num = *nodes_num - 1;
	ae->trig_deflick = 1;

	/* ISP_ERROR("%d\n",*nodes_num); */
	/* for(i=0;i<4;i++){ */
	/*     for(j=0;j<30;j++) */
	/*       ISP_ERROR("%d ",deflick_lut[i*30+j]); */
	/*     ISP_ERROR("\n"); */
	/* } */
	/* ISP_ERROR("\n"); */
}

void tiziano_deflicker_expt_tune(tisp_ae_t *ae, uint32_t pal, uint32_t fps, uint32_t vts, uint32_t hts)
{
	tiziano_deflicker_expt(ae, pal, fps, vts, hts,
			       ae->_deflick_lut, &ae->_nodes_num);
}

void system_reg_write_ae(tisp_ae_t *ae, uint32_t trig, uint32_t addr, uint32_t value){
	if (trig==1){
		system_reg_write(((tisp_core_t *)ae->core)->priv_data, AE_STAT_ADDR_REG_CTRL, 0x1);
	}else if (trig==2){
		system_reg_write(((tisp_core_t *)ae->core)->priv_data, GIB_ADDR_REG_CTRL, 0x1);
	}
	system_reg_write(((tisp_core_t *)ae->core)->priv_data, addr, value);
}

int tiziano_ae_set_hardware_param(tisp_ae_t *ae)
{
	if (ae->ae_first == 0) {
		ae->ae_first = 1;
		system_reg_write_ae(ae, 0, AE_STAT_ADDR_ZONE_NUM_START, ae->_ae_parameter[3 ] << 28 |
				ae->_ae_parameter[2] << 16 | ae->_ae_parameter[1 ] << 12 |
				ae->_ae_parameter[0 ]);
		system_reg_write_ae(ae, 0, AE_STAT_ADDR_HOR_ZONE_SIZE_14, ae->_ae_parameter[7 ] << 24 |
				ae->_ae_parameter[6 ] << 16 | ae->_ae_parameter[5 ] << 8  |
				ae->_ae_parameter[4 ]);
		system_reg_write_ae(ae, 0, AE_STAT_ADDR_HOR_ZONE_SIZE_58, ae->_ae_parameter[11] << 24 |
				ae->_ae_parameter[10] << 16 | ae->_ae_parameter[9 ] << 8  |
				ae->_ae_parameter[8 ]);
		system_reg_write_ae(ae, 0, AE_STAT_ADDR_HOR_ZONE_SIZE_912, ae->_ae_parameter[15] << 24 |
				ae->_ae_parameter[14] << 16 | ae->_ae_parameter[13] << 8  |
				ae->_ae_parameter[12]);
		system_reg_write_ae(ae, 0, AE_STAT_ADDR_HOR_ZONE_SIZE_1315, ae->_ae_parameter[18] << 16 |
				ae->_ae_parameter[17] << 8  | ae->_ae_parameter[16]);
		system_reg_write_ae(ae, 0, AE_STAT_ADDR_VER_ZONE_SIZE_14, ae->_ae_parameter[22] << 24 |
				ae->_ae_parameter[21] << 16 | ae->_ae_parameter[20] << 8  |
				ae->_ae_parameter[19]);
		system_reg_write_ae(ae, 0, AE_STAT_ADDR_VER_ZONE_SIZE_58, ae->_ae_parameter[26] << 24 |
				ae->_ae_parameter[25] << 16 | ae->_ae_parameter[24] << 8  |
				ae->_ae_parameter[23]);
		system_reg_write_ae(ae, 0, AE_STAT_ADDR_VER_ZONE_SIZE_912, ae->_ae_parameter[30] << 24 |
				ae->_ae_parameter[29] << 16 | ae->_ae_parameter[28] << 8  |
				ae->_ae_parameter[27]);
		system_reg_write_ae(ae, 0, AE_STAT_ADDR_VER_ZONE_SIZE_1315, ae->_ae_parameter[33] << 16 |
				ae->_ae_parameter[32] << 8  | ae->_ae_parameter[31]);
	}
	system_reg_write_ae(ae, 1, AE_STAT_ADDR_LUM_TH_FREQ, ae->_ae_parameter[37] << 20 |
			 ae->_ae_parameter[36] << 16 | ae->_ae_parameter[35] << 8  |
			 ae->_ae_parameter[34]);

	return 0;
}


void ae_parameter_init(tisp_ae_t *ae){
	memcpy(ae->_ae_parameter, _ae_parameter, sizeof(_ae_parameter));
	memcpy(ae->ae_switch_night_mode, ae_switch_night_mode, sizeof(ae_switch_night_mode));
	memcpy(ae->_AePointPos, _AePointPos, sizeof(_AePointPos));
	memcpy(ae->_exp_parameter, _exp_parameter, sizeof(_exp_parameter));
	memcpy(ae->ae_ev_step, ae_ev_step, sizeof(ae_ev_step));
	memcpy(ae->ae_stable_tol, ae_stable_tol, sizeof(ae_stable_tol));
	memcpy(ae->_ev_list, _ev_list, sizeof(_ev_list));
	memcpy(ae->_lum_list, _lum_list, sizeof(_lum_list));
	memcpy(ae->_at_list, _at_list, sizeof(_at_list));
	memcpy(ae->_deflicker_para, _deflicker_para, sizeof(_deflicker_para));
	memcpy(ae->_flicker_t, _flicker_t, sizeof(_flicker_t));
	memcpy(ae->_deflick_lut, _deflick_lut, sizeof(_deflick_lut));
	memcpy(ae->_scene_para, _scene_para, sizeof(_scene_para));
	memcpy(ae->ae_scene_mode_th, ae_scene_mode_th, sizeof(ae_scene_mode_th));
	memcpy(ae->_log2_lut, _log2_lut, sizeof(_log2_lut));
	memcpy(ae->_weight_lut, _weight_lut, sizeof(_weight_lut));
	memcpy(ae->_ae_zone_weight, _ae_zone_weight, sizeof(_ae_zone_weight));
	memcpy(ae->_scene_roui_weight, _scene_roui_weight, sizeof(_scene_roui_weight));
	memcpy(ae->_scene_roi_weight, _scene_roi_weight, sizeof(_scene_roi_weight));
	memcpy(ae->_ae_result, _ae_result, sizeof(_ae_result));
	memcpy(ae->_ae_stat, _ae_stat, sizeof(_ae_stat));
	memcpy(ae->_ae_wm_q, _ae_wm_q, sizeof(_ae_wm_q));
	memcpy(ae->ev_cache, ev_cache, sizeof(ev_cache));
	memcpy(ae->ad_cache, ad_cache, sizeof(ad_cache));
	memcpy(ae->ag_cache, ag_cache, sizeof(ag_cache));
	memcpy(ae->dg_cache, dg_cache, sizeof(dg_cache));

}


int tiziano_ae_init(tisp_ae_t *ae, uint32_t height,uint32_t width,tisp_ae_ex_min_t ae_init)
{
	int32_t i;
	uint32_t again;
	uint32_t Sdgain;



	tisp_core_t *core = ae->core;

	/*private_variable init*/
	ae->ftune = 1;
	ae->ae_first = 0;
	ae->min_it = ae_init.min_it;
	ae->min_ag = 1024;
	ae->min_dg = 1024;
	memset(&ae->tisp_ae_hist, 0, sizeof(ae->tisp_ae_hist));
	ae->tisp_ae_hist.ae_hist_hv[0] = 15;
	ae->tisp_ae_hist.ae_hist_hv[1] = 15;
	ae->tisp_ae_hist.ae_hist_nodes[0] = 13;
	ae->tisp_ae_hist.ae_hist_nodes[1] = 64;
	ae->tisp_ae_hist.ae_hist_nodes[2] = 144;
	ae->tisp_ae_hist.ae_hist_nodes[3] = 192;
	memcpy(&ae->tisp_ae_hist_last, &ae->tisp_ae_hist, sizeof(ae->tisp_ae_hist));

	memset(ae->ae_array_d, 0, sizeof(ae->ae_array_d));
	memset(ae->ae_array_m, 0, sizeof(ae->ae_array_m));
	memset(ae->ae_array_s, 0, sizeof(ae->ae_array_s));
	memset(ae->ae_array_ir, 0, sizeof(ae->ae_array_ir));
	memset(ae->ae_array_dc, 0, sizeof(ae->ae_array_dc));
	memset(ae->ae_array_sc, 0, sizeof(ae->ae_array_sc));

	memset(ae->ae_hist_array, 0, sizeof(ae->ae_hist_array));
	memset(ae->ae_hist_ir_array, 0, sizeof(ae->ae_hist_ir_array));

	memset(ae->_ae_reg, 0, sizeof(ae->_ae_reg));
	ae->_ae_ev = 0;
	ae->ae_dn_refresh_flag = 0;
	ae->ae_compensation = 128;
	ae->ae_ev_init_strict = 500;
	ae->ae_ev_init_en = 0;

	ae->trig = 0;
	ae->force_trig = 0;
	ae->trig_deflick = 0;
	ae->trig_cal = 1;
	ae->again_old = 0;
	ae->again_new = 0;
	ae->total_gain_old = 0;
	ae->total_gain_new = 0;
	ae->ag_old = 0;
	ae->dg_old = 0;
	ae->ag_new = 0x400;
	ae->dg_new = 0x400;
	ae->EffectFrame = 0;
	ae->EffectCount = 0;

	ae->_nodes_num = 119;

	ae_parameter_init(ae);

	tiziano_ae_params_refresh(ae);
	for (i = 0; i < ae->_ae_parameter[3]; i++) {
		ae->_ae_parameter[4 + i] = (width / 2) / ae->_ae_parameter[3];
	}
	for (i = 0; i < ae->_ae_parameter[1]; i++) {
		ae->_ae_parameter[19 + i] = (height / 2) / ae->_ae_parameter[1];
	}

	tiziano_ae_set_hardware_param(ae);

	if (tisp_set_sensor_integration_time(ae, ae->_ae_result[2])) { //expt
		ISP_ERROR("sorry,set integration time failed!\n");
		return -1;
	}
	tisp_set_sensor_analog_gain(ae, ae->_ae_result[3]); // sensor ag
	/*tisp_set_sensor_digital_gain(_ae_result[3]); // sensor dg*/
	system_reg_write_ae(ae, 2, GIB_ADDR_R_G, ae->_ae_result[1] << 16 | ae->_ae_result[1]); //isp dg
	system_reg_write_ae(ae, 2, GIB_ADDR_B_IR, ae->_ae_result[1] << 16 | ae->_ae_result[1]);//isp dg

	system_irq_func_set(((tisp_core_t *)ae->core)->priv_data, TIZIANO_ISP_IRQ_AE_HIST, ae_interrupt_hist, ae);
	//tiziano_ae_dump();
	system_irq_func_set(((tisp_core_t *)ae->core)->priv_data, TIZIANO_ISP_IRQ_AE_STATIC, ae_interrupt_static, ae);

	memset(&ae->tisp_ae_ctrls, 0, sizeof(ae->tisp_ae_ctrls));
	memset(&ae->ae_ctrls, 0, sizeof(ae->ae_ctrls));

	if (0 == ae->_exp_parameter[2]) {
		ae->_exp_parameter[2] = core->sensor_ctrl.param.integration_time_max;
	} else if (core->sensor_ctrl.param.integration_time_max < ae->_exp_parameter[2]) {
		ae->_exp_parameter[2] = core->sensor_ctrl.param.integration_time_max;
	}
	ae->tisp_ae_ctrls.tisp_ae_max_sensor_integration_time = ae->_exp_parameter[2];

	again = tisp_math_exp2(core->sensor_ctrl.param.again_log2_max, 16, 10);
	if (0 == ae->_exp_parameter[3]) {
		ae->_exp_parameter[3] = again;
	} else if (again < ae->_exp_parameter[3]) {
		ae->_exp_parameter[3] = again;
	}

	Sdgain = tisp_math_exp2(core->sensor_ctrl.param.dgain_log2_max, 16, 10);

	ae->tisp_ae_ctrls.tisp_ae_max_sensor_again = ae->_exp_parameter[3];
	ae->tisp_ae_ctrls.tisp_ae_max_isp_dgain = ae->_exp_parameter[4];
	ae->tisp_ae_ctrls.tisp_ae_max_sensor_dgain = Sdgain;

	tiziano_deflicker_expt(ae, ae->_flicker_t[0], ae->_flicker_t[1],
			       ae->_flicker_t[2], ae->_flicker_t[3],
			       ae->_deflick_lut, &ae->_nodes_num);


#ifdef TISP_AE_PROCESS_THREAD
	tisp_event_set_cb(&core->tisp_event_mg, TISP_EVENT_TYPE_AE_PROCESS, tisp_ae_process, ae);
#else
#endif
	spin_lock_init(&ae->slock);
	spin_lock_init(&ae->slock_hist);

	return 0;
}

int tisp_ae_manual_set(tisp_ae_t *ae, tisp_ae_ctrls_t ae_manual)
{
	ae->tisp_ae_ctrls.tisp_ae_it_manual = ae_manual.tisp_ae_it_manual;
	ae->tisp_ae_ctrls.tisp_ae_sensor_integration_time = ae_manual.tisp_ae_sensor_integration_time;
	ae->trig_cal = 0;

	return 0;
}

int tisp_ae_get_y_zone(tisp_ae_t *ae, tisp_zone_info_t *ae_zone)
{
	unsigned long flags = 0;

	spin_lock_irqsave(&ae->slock, flags);
	memcpy(ae_zone, &ae->y_zone_last, sizeof(ae->y_zone));
	spin_unlock_irqrestore(&ae->slock, flags);

	return 0;
}

int32_t tisp_ae_param_array_get(tisp_ae_t *ae, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_AE_PARAMETER:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_PARAMETER);
		tmpbuf = ae->_ae_parameter;
		break;
	case TISP_PARAM_AE_SWITCH_NIGHT_MODE:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_SWITCH_NIGHT_MODE);
		tmpbuf = ae->ae_switch_night_mode;
		break;
	case TISP_PARAM_AE_POINTPOS:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_POINTPOS);
		tmpbuf = ae->_AePointPos;
		break;
	case TISP_PARAM_AE_EXP_PARAMETER:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_EXP_PARAMETER);
		tmpbuf = ae->_exp_parameter;
		break;
	case TISP_PARAM_AE_EV_STEP:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_EV_STEP);
		tmpbuf = ae->ae_ev_step;
		break;
	case TISP_PARAM_AE_STABLE_TOL:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_STABLE_TOL);
		tmpbuf = ae->ae_stable_tol;
		break;
	case TISP_PARAM_AE_EV_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_EV_LIST);
		tmpbuf = ae->_ev_list;
		break;
	case TISP_PARAM_AE_LUM_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_LUM_LIST);
		tmpbuf = ae->_lum_list;
		break;
	case TISP_PARAM_AE_AT_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_AT_LIST);
		tmpbuf = ae->_at_list;
		break;
	case TISP_PARAM_AE_DEFLICKER_PARA:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_DEFLICKER_PARA);
		tmpbuf = ae->_deflicker_para;
		break;
	case TISP_PARAM_AE_FLICKER_T:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_FLICKER_T);
		tmpbuf = ae->_flicker_t;
		break;
	case TISP_PARAM_AE_SCENE_PARE:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_SCENE_PARE);
		tmpbuf = ae->_scene_para;
		break;
	case TISP_PARAM_AE_SCENE_MODE_TH:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_SCENE_MODE_TH);
		tmpbuf = ae->ae_scene_mode_th;
		break;
	case TISP_PARAM_AE_ROI_TREND_LOG2_LUT:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_ROI_TREND_LOG2_LUT);
		tmpbuf = ae->_log2_lut;
		break;
	case TISP_PARAM_AE_ROI_TREND_WEIGHT_LUT:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_ROI_TREND_WEIGHT_LUT);
		tmpbuf = ae->_weight_lut;
		break;
	case TISP_PARAM_AE_ZONE_WEIGHT:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_ZONE_WEIGHT);
		tmpbuf = ae->_ae_zone_weight;
		break;
	case TISP_PARAM_AE_SCENE_ROUI_WEIGHT:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_SCENE_ROUI_WEIGHT);
		tmpbuf = ae->_scene_roui_weight;
		break;
	case TISP_PARAM_AE_SCENE_ROI_WEIGHT:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_SCENE_ROI_WEIGHT);
		tmpbuf = ae->_scene_roi_weight;
		break;
	case TISP_PARAM_AE_RESULT:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_RESULT);
		tmpbuf = ae->_ae_result;
		break;
	case TISP_PARAM_AE_STAT:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_STAT);
		tmpbuf = ae->_ae_stat;
		break;
	case TISP_PARAM_AE_WM_Q:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_WM_Q);
		tmpbuf = ae->_ae_wm_q;
		break;
	default:
		ISP_ERROR("%s,%d: ae not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	memcpy(buf, tmpbuf, len);
	*plen = len;

	return 0;
}

int32_t tisp_ae_param_array_set(tisp_ae_t *ae, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_AE_PARAMETER:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_PARAMETER);
		tmpbuf = ae->_ae_parameter;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AE_SWITCH_NIGHT_MODE:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_SWITCH_NIGHT_MODE);
		tmpbuf = ae->ae_switch_night_mode;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AE_POINTPOS:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_POINTPOS);
		tmpbuf = ae->_AePointPos;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AE_EXP_PARAMETER:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_EXP_PARAMETER);
		tmpbuf = ae->_exp_parameter;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AE_EV_STEP:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_EV_STEP);
		tmpbuf = ae->ae_ev_step;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AE_STABLE_TOL:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_STABLE_TOL);
		tmpbuf = ae->ae_stable_tol;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AE_EV_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_EV_LIST);
		tmpbuf = ae->_ev_list;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AE_LUM_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_LUM_LIST);
		tmpbuf = ae->_lum_list;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AE_AT_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_AT_LIST);
		tmpbuf = ae->_at_list;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AE_DEFLICKER_PARA:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_DEFLICKER_PARA);
		tmpbuf = ae->_deflicker_para;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AE_FLICKER_T:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_FLICKER_T);
		tmpbuf = ae->_flicker_t;
		memcpy(tmpbuf, buf, len);
		tiziano_deflicker_expt(ae, ae->_flicker_t[0], ae->_flicker_t[1], ae->_flicker_t[2], ae->_flicker_t[3], ae->_deflick_lut, &ae->_nodes_num);
		break;
	case TISP_PARAM_AE_SCENE_PARE:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_SCENE_PARE);
		tmpbuf = ae->_scene_para;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AE_SCENE_MODE_TH:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_SCENE_MODE_TH);
		tmpbuf = ae->ae_scene_mode_th;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AE_ROI_TREND_LOG2_LUT:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_ROI_TREND_LOG2_LUT);
		tmpbuf = ae->_log2_lut;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AE_ROI_TREND_WEIGHT_LUT:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_ROI_TREND_WEIGHT_LUT);
		tmpbuf = ae->_weight_lut;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AE_ZONE_WEIGHT:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_ZONE_WEIGHT);
		tmpbuf = ae->_ae_zone_weight;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AE_SCENE_ROUI_WEIGHT:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_SCENE_ROUI_WEIGHT);
		tmpbuf = ae->_scene_roui_weight;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AE_SCENE_ROI_WEIGHT:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_SCENE_ROI_WEIGHT);
		tmpbuf = ae->_scene_roi_weight;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_AE_RESULT:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_RESULT);
		tmpbuf = ae->_ae_result;
		break;
	case TISP_PARAM_AE_STAT:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_STAT);
		tmpbuf = ae->_ae_stat;
		break;
	case TISP_PARAM_AE_WM_Q:
		len = sizeof(tparams.params_data.TISP_PARAM_AE_WM_Q);
		tmpbuf = ae->_ae_wm_q;
		break;
	default:
		ISP_ERROR("%s,%d: ae not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}
	if (*plen != len) {
		ISP_ERROR("%s,%d: param size error %d, want len = %d, recv len = %d\n",
		       __func__, __LINE__, id, len, *plen);
		return -1;
	}

	ae->trig = 1;
	ae->force_trig = 1;
	tiziano_ae_set_hardware_param(ae);

	return 0;
}
