#include <linux/kernel.h>
#include <linux/irqreturn.h>
#include <linux/dma-mapping.h>

#include "fix_point_calc.h"
#include "../inc/tiziano_sys.h"
#include "tiziano_core.h"
#include "tiziano_gib.h"
#include "tiziano_params.h"
#include "tiziano_map.h"
#include "../inc/tiziano_core_tuning.h"

#define ISPGIB_SUBABS(A,B)     ((B>A)?(B-A):(A-B))
//static uint32_t trig_set_deir = 0;

uint32_t tiziano_gib_config_line[6];
uint32_t tiziano_gib_r_g_linear[2];
uint32_t tiziano_gib_b_ir_linear[2];

uint32_t tiziano_gib_deirm_blc_r_linear[9] = {253, 254, 256, 258, 258, 258, 258, 258, 258}; //R  1x_gain ~ 256x_gain
uint32_t tiziano_gib_deirm_blc_gr_linear[9] = {253, 254, 256, 258, 259, 259, 259, 259, 259};//Gr 1x_gain ~ 256x_gain
uint32_t tiziano_gib_deirm_blc_gb_linear[9] = {253, 254, 254, 257, 257, 257, 257, 257, 257}; //Gb 1x_gain ~ 256x_gain
uint32_t tiziano_gib_deirm_blc_b_linear[9] = {253, 254, 254, 257, 256, 256, 256, 256, 256}; //B  1x_gain ~ 256x_gain
uint32_t tiziano_gib_deirm_blc_ir_linear[9] = {65, 63, 67, 66, 63, 63, 63, 63, 63}; //B  1x_gain ~ 256x_gain

//uint32_t gib_ir_mode[2] = {1, 0};
uint32_t gib_ir_value[2] = {45, 45};  //gib_ir, gib_ir_last,
uint32_t gib_ir_point[4] = {5,50,51,128};
//940nm:5,50,80,100 ; 5,45,60,85;
//850nm:5,64,100,120;
uint32_t gib_ir_reser[15] = {0}; //gib_zir_thes;
uint32_t tiziano_gib_deir_r_h[33] = {0};
//={0,77,154,230,307,384,461,538,614,691,768,845,922,998,1075,1152,1229,1306,1382,1459,1536,1613,1690,1766,1843,1920,1997,2074,2150,2227,2304,2381,2458};
uint32_t tiziano_gib_deir_g_h[33] = {0};
//={0,108,215,323,430,538,645,753,860,968,1075,1183,1290,1398,1505,1613,1720,1828,1935,2043,2150,2258,2365,2473,2580,2688,2796,2903,3011,3118,3226,3333,3441};
uint32_t tiziano_gib_deir_b_h[33] = {0};
//={0,138,276,415,553,691,829,968,1106,1244,1382,1521,1659,1797,1935,2074,2212,2350,2488,2627,2765,2903,3041,3180,3318,3456,3594,3732,3871,4009,4095,4095,4095};

uint32_t tiziano_gib_deir_r_m[33] = {0,64,128,192,256,320,384,448,512,576,640,704,768,832,896,960,1024,1088,1152,1216,1280,1344,1408,1472,1536,1600,1664,1728,1792,1856,1920,1984,2048};//method 1
//={0,271,353,459,589,738,904,1084,1276,1477,1683,1893,2103,2310,2512,2889,3059,3212,3346,3458,3545,3605,3635,3635,3635,3635,3635,3635,3635,3635,3635,3635,3635};
//method 2  //fish
//={0,106,212,319,425,531,637,744,850,956,1062,1169,1275,1381,1487,1594,1700,1806,1912,2019,2125,2231,2337,2444,2550,2656,2762,2868,2975,3081,3187,3293,3400};
//fish update1: mode0, 0.1, 0.1, 0.1

//original 850nm
//={0,77,154,230,307,384,461,538,614,691,768,845,922,998,1075,1152,1229,1306,1382,1459,1536,1613,1690,1766,1843,1920,1997,2074,2150,2227,2304,2381,2458};

uint32_t tiziano_gib_deir_g_m[33] = {0,90,179,269,358,448,538,627,717,806,896,986,1075,1165,1254,1344,1434,1523,1613,1702,1792,1882,1971,2061,2150,2240,2330,2419,2509,2598,2688,2778,2867};//={0,145,262,383,506,633,762,893, 1026,1162,1299,1437,1576,1717,1858,2141,2282,2423,2564,2704,2843,2981,3117,3117,3117,3117,3117,3117,3117,3117,3117,3117,3117};
//={0,118,236,353,471,589,707,824,942,1060,1178,1295,1413,1531,1649,1766,1884,2002,2120,2237,2355,2473,2591,2708,2826,2944,3062,3180,3297,3415,3533,3651,3768};
//={0,26,51,77,102,128,154,179,205,230,256,282,307,333,358,384,410,435,461,486,512,538,563,589,614,640,666,691,717,742,768,794,819};

uint32_t tiziano_gib_deir_b_m[33] = {0,115,230,346,461,576,691,806,922,1037,1152,1267,1382,1498,1613,1728,1843,1958,2074,2189,2304,2419,2534,2650,2765,2880,2995,3110,3226,3341,3456,3571,3686};//={0,112,229,346,463,581,699,817, 936, 1055,1173,1292,1411,1530,1649,1886,2004,2122,2240,2358,2475,2592,2708,2708,2708,2708,2708,2708,2708,2708,2708,2708,2708};
//={0,118,236,353,471,589,707,824,942,1060,1178,1295,1413,1531,1649,1766,1884,2002,2120,2237,2355,2473,2591,2708,2826,2944,3062,3180,3297,3415,3533,3651,3768};
//={0,90,179,269,358,448,538,627,717,806,896,986,1075,1165,1254,1344,1434,1523,1613,1702,1792,1882,1971,2061,2150,2240,2330,2419,2509,2598,2688,2778,2867};

uint32_t tiziano_gib_deir_r_l[33] = {0};
//={0,64,128,192,256,320,384,448,512,576,640,704,768,832,896,960,1024,1088,1152,1216,1280,1344,1408,1472,1536,1600,1664,1728,1792,1856,1920,1984,2048};
uint32_t tiziano_gib_deir_g_l[33] = {0};
//={0,90,179,269,358,448,538,627,717,806,896,986,1075,1165,1254,1344,1434,1523,1613,1702,1792,1882,1971,2061,2150,2240,2330,2419,2509,2598,2688,2778,2867};
uint32_t tiziano_gib_deir_b_l[33] = {0};
//={0,115,230,346,461,576,691,806,922,1037,1152,1267,1382,1498,1613,1728,1843,1958,2074,2189,2304,2419,2534,2650,2765,2880,2995,3110,3226,3341,3456,3571,3686};

uint32_t tiziano_gib_deir_matrix_h[15] = {0};
uint32_t tiziano_gib_deir_matrix_m[15] = {0};
uint32_t tiziano_gib_deir_matrix_l[15] = {0};
/****************** Code *************************/

int32_t tisp_gib_gain_interpolation(tisp_gib_t *gib, uint32_t ag_value)
{
	int32_t ag_value_int, ag_value_fra;
	uint32_t blc_r, blc_gr, blc_gb, blc_b,blc_ir;

	ag_value_int = ag_value >>16;
	ag_value_fra = ag_value & 0xffff;

	blc_r  = tisp_simple_intp(ag_value_int, ag_value_fra, gib->tiziano_gib_deirm_blc_r_linear);
	blc_gr = tisp_simple_intp(ag_value_int, ag_value_fra, gib->tiziano_gib_deirm_blc_gr_linear);
	blc_gb = tisp_simple_intp(ag_value_int, ag_value_fra, gib->tiziano_gib_deirm_blc_gb_linear);
	blc_b  = tisp_simple_intp(ag_value_int, ag_value_fra, gib->tiziano_gib_deirm_blc_b_linear);
	blc_ir = tisp_simple_intp(ag_value_int, ag_value_fra, gib->tiziano_gib_deirm_blc_ir_linear);

	system_reg_write(((tisp_core_t *)gib->core)->priv_data, GIB_ADDR_DEIRM_BLC12, (blc_gr << 16) | blc_r);
	system_reg_write(((tisp_core_t *)gib->core)->priv_data, GIB_ADDR_DEIRM_BLC34, (blc_b << 16) | blc_gb);
	system_reg_write(((tisp_core_t *)gib->core)->priv_data, GIB_ADDR_DEIRM_BLC5, blc_ir);

	return 0;
}

int tiziano_gib_lut_parameter(tisp_gib_t *gib)
{
	static int init = 0;

	system_reg_write(((tisp_core_t *)gib->core)->priv_data, GIB_ADDR_CONFIG, (gib->tiziano_gib_config_line[0] << 20) |
			 (gib->tiziano_gib_config_line[1] << 16) |
			 (gib->tiziano_gib_config_line[2] << 12) |
			 (gib->tiziano_gib_config_line[3] << 8) |
			 (gib->tiziano_gib_config_line[4] << 4) |
			 gib->tiziano_gib_config_line[5]);
	system_reg_write(((tisp_core_t *)gib->core)->priv_data, GIB_ADDR_DEIRM_BLC12,
			 (gib->tiziano_gib_deirm_blc_gr_linear[0]<<16) |
			 gib->tiziano_gib_deirm_blc_r_linear[0]);
	system_reg_write(((tisp_core_t *)gib->core)->priv_data, GIB_ADDR_DEIRM_BLC34,
			 (gib->tiziano_gib_deirm_blc_b_linear[0] <<16) |
			 gib->tiziano_gib_deirm_blc_gb_linear[0]);
	system_reg_write(((tisp_core_t *)gib->core)->priv_data, GIB_ADDR_DEIRM_BLC5,
			 gib->tiziano_gib_deirm_blc_ir_linear[0]);

	if (init == 0){
		system_reg_write(((tisp_core_t *)gib->core)->priv_data, GIB_ADDR_R_G, gib->tiziano_gib_r_g_linear[0]
				 | (gib->tiziano_gib_r_g_linear[1]  << 16));
		system_reg_write(((tisp_core_t *)gib->core)->priv_data, GIB_ADDR_B_IR, gib->tiziano_gib_b_ir_linear[0]
				 | (gib->tiziano_gib_b_ir_linear[1] << 16));
		init = 1;
	}

	return 0;
}

int tiziano_gib_params_refresh(tisp_gib_t *gib)
{
	memcpy(gib->tiziano_gib_config_line,
	       tparams.params_data.TISP_PARAM_GIB_CONFIG_LINE,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_CONFIG_LINE));
	memcpy(gib->tiziano_gib_r_g_linear,
	       tparams.params_data.TISP_PARAM_GIB_R_G_LINEAR,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_R_G_LINEAR));
	memcpy(gib->tiziano_gib_b_ir_linear,
	       tparams.params_data.TISP_PARAM_GIB_B_IR_LINEAR,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_B_IR_LINEAR));
	memcpy(gib->tiziano_gib_deirm_blc_r_linear,
	       tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_R_LINEAR,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_R_LINEAR));
	memcpy(gib->tiziano_gib_deirm_blc_gr_linear,
	       tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_GR_LINEAR,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_GR_LINEAR));
	memcpy(gib->tiziano_gib_deirm_blc_gb_linear,
	       tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_GB_LINEAR,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_GB_LINEAR));
	memcpy(gib->tiziano_gib_deirm_blc_b_linear,
	       tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_B_LINEAR,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_B_LINEAR));
	memcpy(gib->tiziano_gib_deirm_blc_ir_linear,
	       tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_IR_LINEAR,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_IR_LINEAR));
	memcpy(gib->gib_ir_point,
	       tparams.params_data.TISP_PARAM_GIB_IR_POINT,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_IR_POINT));
	memcpy(gib->gib_ir_reser,
	       tparams.params_data.TISP_PARAM_GIB_IR_RESER,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_IR_RESER));
	memcpy(gib->tiziano_gib_deir_r_h,
	       tparams.params_data.TISP_PARAM_GIB_DEIR_R_H,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_R_H));
	memcpy(gib->tiziano_gib_deir_g_h,
	       tparams.params_data.TISP_PARAM_GIB_DEIR_G_H,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_G_H));
	memcpy(gib->tiziano_gib_deir_b_h,
	       tparams.params_data.TISP_PARAM_GIB_DEIR_B_H,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_B_H));
	memcpy(gib->tiziano_gib_deir_r_m,
	       tparams.params_data.TISP_PARAM_GIB_DEIR_R_M,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_R_M));
	memcpy(gib->tiziano_gib_deir_g_m,
	       tparams.params_data.TISP_PARAM_GIB_DEIR_G_M,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_G_M));
	memcpy(gib->tiziano_gib_deir_b_m,
	       tparams.params_data.TISP_PARAM_GIB_DEIR_B_M,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_B_M));
	memcpy(gib->tiziano_gib_deir_r_l,
	       tparams.params_data.TISP_PARAM_GIB_DEIR_R_L,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_R_L));
	memcpy(gib->tiziano_gib_deir_g_l,
	       tparams.params_data.TISP_PARAM_GIB_DEIR_G_L,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_G_L));
	memcpy(gib->tiziano_gib_deir_b_l,
	       tparams.params_data.TISP_PARAM_GIB_DEIR_B_L,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_B_L));
	memcpy(gib->tiziano_gib_deir_matrix_h,
	       tparams.params_data.TISP_PARAM_GIB_DEIR_MATRIX_H,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_MATRIX_H));
	memcpy(gib->tiziano_gib_deir_matrix_m,
	       tparams.params_data.TISP_PARAM_GIB_DEIR_MATRIX_M,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_MATRIX_M));
	memcpy(gib->tiziano_gib_deir_matrix_l,
	       tparams.params_data.TISP_PARAM_GIB_DEIR_MATRIX_L,
	       sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_MATRIX_L));

	return 0;
}

int tiziano_gib_dn_params_refresh(tisp_gib_t *gib)
{
	tisp_core_t *core = gib->core;
	tiziano_gib_params_refresh(gib);

	if (core->deir_en == 1) {
		if (core->core_tuning.day_night == 0)
			gib->tiziano_gib_config_line[3] = 1;
		else
			gib->tiziano_gib_config_line[3] = 0;
	} else {
		gib->tiziano_gib_config_line[3] = 0;
	}
	tiziano_gib_lut_parameter(gib);

	return 0;
}

void tiziano_gib_deir_reg(tisp_gib_t *gib,
			  uint32_t *tiziano_gib_deir_r,
			  uint32_t *tiziano_gib_deir_g,
			  uint32_t *tiziano_gib_deir_b)
{
	int i;

	for (i = 0; i < 32; i++) {
		/* ISP_ERROR("r_mem %08x,g_mem %08x,b_mem %08x\n",GIB_DEIR_R_BASE + i*4,GIB_DEIR_G_BASE + i*4,GIB_DEIR_B_BASE + i*4); */
		system_reg_write(((tisp_core_t *)gib->core)->priv_data, GIB_DEIR_R_BASE + i * 4,
				 (tiziano_gib_deir_r[i+1]<<12) | tiziano_gib_deir_r[i]);
		system_reg_write(((tisp_core_t *)gib->core)->priv_data, GIB_DEIR_G_BASE + i * 4,
				 (tiziano_gib_deir_g[i+1]<<12) | tiziano_gib_deir_g[i]);
		system_reg_write(((tisp_core_t *)gib->core)->priv_data, GIB_DEIR_B_BASE + i * 4,
				 (tiziano_gib_deir_b[i+1]<<12) | tiziano_gib_deir_b[i]);
	}
}

void tiziano_gib_deir_interpolate(uint32_t *gib_deir_rgb, uint32_t gib_ir,
				  uint32_t point_h, uint32_t point_l,
				  uint32_t *value_h, uint32_t *value_l)
{
	int i;

	if (point_h == point_l)
		point_h = point_l + 1;

	for (i = 0; i < 33; i++) {
		gib_deir_rgb[i] = (value_l[i] * (point_h-point_l) +
				   (value_h[i]-value_l[i]) * (gib_ir-point_l)) / (point_h-point_l);
	}
}

void tiziano_gib_deir_ir_interpolation(tisp_gib_t *gib, int32_t gib_ir)
{
	static uint32_t deir_flag = 0;
	static uint32_t deir_flag_last = 0;
	uint32_t deir_low_r = gib->gib_ir_point[0];
	uint32_t deir_med_l = gib->gib_ir_point[1];
	uint32_t deir_med_r = gib->gib_ir_point[2];
	uint32_t deir_hig_l = gib->gib_ir_point[3];

	static uint32_t gib_deir_r[33];
	static uint32_t gib_deir_g[33];
	static uint32_t gib_deir_b[33];

	if (gib_ir > deir_hig_l)
		deir_flag = 0;                                      // High
	else if ((gib_ir <= deir_hig_l) && (gib_ir > deir_med_r))
		deir_flag = 1;                                      //interpolation
	else if ((gib_ir <= deir_med_r) && (gib_ir > deir_med_l))
		deir_flag = 2;                                      //Median
	else if ((gib_ir <= deir_med_l) && (gib_ir > deir_low_r))
		deir_flag = 3;                                      //interpolation
	else if (gib_ir <= deir_low_r)
		deir_flag = 4;                                      //Low

	if ((deir_flag == 0 && deir_flag_last == 0) || (deir_flag == 2 && deir_flag_last == 2) || (deir_flag == 4 && deir_flag_last == 4)){
		return;
	}

	deir_flag_last = deir_flag;

	switch (deir_flag) {
	case 0:
		tiziano_gib_deir_reg(gib, gib->tiziano_gib_deir_r_h, gib->tiziano_gib_deir_g_h, gib->tiziano_gib_deir_b_h);
		break;
	case 1:
		tiziano_gib_deir_interpolate(gib_deir_r, gib_ir, deir_hig_l,
					     deir_med_r, gib->tiziano_gib_deir_r_h, gib->tiziano_gib_deir_r_m);
		tiziano_gib_deir_interpolate(gib_deir_g, gib_ir, deir_hig_l,
					     deir_med_r, gib->tiziano_gib_deir_g_h, gib->tiziano_gib_deir_g_m);
		tiziano_gib_deir_interpolate(gib_deir_b, gib_ir, deir_hig_l,
					     deir_med_r, gib->tiziano_gib_deir_b_h, gib->tiziano_gib_deir_b_m);
		tiziano_gib_deir_reg(gib, gib_deir_r, gib_deir_g, gib_deir_b);
		break;
	case 2:
		tiziano_gib_deir_reg(gib, gib->tiziano_gib_deir_r_m,
				     gib->tiziano_gib_deir_g_m, gib->tiziano_gib_deir_b_m);
		break;
	case 3:
		tiziano_gib_deir_interpolate(gib_deir_r, gib_ir, deir_med_l,
					     deir_low_r, gib->tiziano_gib_deir_r_m, gib->tiziano_gib_deir_r_l);
		tiziano_gib_deir_interpolate(gib_deir_g, gib_ir, deir_med_l,
					     deir_low_r, gib->tiziano_gib_deir_g_m, gib->tiziano_gib_deir_g_l);
		tiziano_gib_deir_interpolate(gib_deir_b, gib_ir, deir_med_l,
					     deir_low_r, gib->tiziano_gib_deir_b_m, gib->tiziano_gib_deir_b_l);
		tiziano_gib_deir_reg(gib, gib_deir_r, gib_deir_g, gib_deir_b);
		break;
	case 4:
		tiziano_gib_deir_reg(gib, gib->tiziano_gib_deir_r_l,
				     gib->tiziano_gib_deir_g_l, gib->tiziano_gib_deir_b_l);
		break;
	default:
		break;
	}
}

int32_t tisp_gib_deir_ir_update(tisp_gib_t *gib, uint64_t data1)
{
	uint32_t gib_deir_interpolate_en = 1;
	uint32_t gib_deir_thres = 0;

	gib->gib_ir_value[0] = data1;

	if (gib->tiziano_gib_config_line[3] == 1 && gib_deir_interpolate_en == 1) {
		if (ISPGIB_SUBABS(gib->gib_ir_value[0],gib->gib_ir_value[1])>gib_deir_thres || gib->trig_set_deir == 1) {
			gib->trig_set_deir = 0;
			tiziano_gib_deir_ir_interpolation(gib, gib->gib_ir_value[0]);
			gib->gib_ir_value[1] = gib->gib_ir_value[0];
			//ISP_ERROR("gib_ir %d  gib_ir_last %d \n", gib->gib_ir_value[0],gib->gib_ir_value[1]);
		}
	}

	return 0;
}

void gib_parameter_init(tisp_gib_t *gib)
{
	memcpy(gib->tiziano_gib_config_line, tiziano_gib_config_line, sizeof(tiziano_gib_config_line));
	memcpy(gib->tiziano_gib_r_g_linear, tiziano_gib_r_g_linear, sizeof(tiziano_gib_r_g_linear));
	memcpy(gib->tiziano_gib_b_ir_linear, tiziano_gib_b_ir_linear, sizeof(tiziano_gib_b_ir_linear));

	memcpy(gib->tiziano_gib_deirm_blc_r_linear, tiziano_gib_deirm_blc_r_linear, sizeof(tiziano_gib_deirm_blc_r_linear));
	memcpy(gib->tiziano_gib_deirm_blc_gr_linear, tiziano_gib_deirm_blc_gr_linear, sizeof(tiziano_gib_deirm_blc_gr_linear));
	memcpy(gib->tiziano_gib_deirm_blc_gb_linear, tiziano_gib_deirm_blc_gb_linear, sizeof(tiziano_gib_deirm_blc_gb_linear));
	memcpy(gib->tiziano_gib_deirm_blc_b_linear, tiziano_gib_deirm_blc_b_linear, sizeof(tiziano_gib_deirm_blc_b_linear));
	memcpy(gib->tiziano_gib_deirm_blc_ir_linear, tiziano_gib_deirm_blc_ir_linear, sizeof(tiziano_gib_deirm_blc_ir_linear));

	memcpy(gib->gib_ir_value, gib_ir_value, sizeof(gib_ir_value));
	memcpy(gib->gib_ir_point, gib_ir_point, sizeof(gib_ir_point));
	memcpy(gib->gib_ir_reser, gib_ir_reser, sizeof(gib_ir_reser));
	memcpy(gib->tiziano_gib_deir_r_h, tiziano_gib_deir_r_h, sizeof(tiziano_gib_deir_r_h));
	memcpy(gib->tiziano_gib_deir_g_h, tiziano_gib_deir_g_h, sizeof(tiziano_gib_deir_g_h));
	memcpy(gib->tiziano_gib_deir_b_h, tiziano_gib_deir_b_h, sizeof(tiziano_gib_deir_b_h));
	memcpy(gib->tiziano_gib_deir_r_m, tiziano_gib_deir_r_m, sizeof(tiziano_gib_deir_r_m));
	memcpy(gib->tiziano_gib_deir_g_m, tiziano_gib_deir_g_m, sizeof(tiziano_gib_deir_g_m));
	memcpy(gib->tiziano_gib_deir_b_m, tiziano_gib_deir_b_m, sizeof(tiziano_gib_deir_b_m));
	memcpy(gib->tiziano_gib_deir_r_l, tiziano_gib_deir_r_l, sizeof(tiziano_gib_deir_r_l));
	memcpy(gib->tiziano_gib_deir_g_l, tiziano_gib_deir_g_l, sizeof(tiziano_gib_deir_g_l));
	memcpy(gib->tiziano_gib_deir_b_l, tiziano_gib_deir_b_l, sizeof(tiziano_gib_deir_b_l));

	memcpy(gib->tiziano_gib_deir_matrix_h, tiziano_gib_deir_matrix_h, sizeof(tiziano_gib_deir_matrix_h));
	memcpy(gib->tiziano_gib_deir_matrix_m, tiziano_gib_deir_matrix_m, sizeof(tiziano_gib_deir_matrix_m));
	memcpy(gib->tiziano_gib_deir_matrix_l, tiziano_gib_deir_matrix_l, sizeof(tiziano_gib_deir_matrix_l));
}

int tiziano_gib_init(tisp_gib_t *gib)
{
	tisp_core_t *core = gib->core;

	gib->trig_set_deir = 0;
	gib_parameter_init(gib);

	tiziano_gib_params_refresh(gib);

	if (core->deir_en == 1) {
		if (core->core_tuning.day_night == 0)
			gib->tiziano_gib_config_line[3] = 1;
		else
			gib->tiziano_gib_config_line[3] = 0;
	} else {
		gib->tiziano_gib_config_line[3] = 0;
	}
	tiziano_gib_lut_parameter(gib);
	tiziano_gib_deir_reg(gib, gib->tiziano_gib_deir_r_m,
			     gib->tiziano_gib_deir_g_m,
			     gib->tiziano_gib_deir_b_m);

	return 0;
}

int32_t tisp_gib_param_array_get(tisp_gib_t *gib, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_GIB_CONFIG_LINE:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_CONFIG_LINE);
		tmpbuf = gib->tiziano_gib_config_line;
		break;
	case TISP_PARAM_GIB_R_G_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_R_G_LINEAR);
		tmpbuf = gib->tiziano_gib_r_g_linear;
		break;
	case TISP_PARAM_GIB_B_IR_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_B_IR_LINEAR);
		tmpbuf = gib->tiziano_gib_b_ir_linear;
		break;
	case TISP_PARAM_GIB_DEIRM_BLC_R_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_R_LINEAR);
		tmpbuf = gib->tiziano_gib_deirm_blc_r_linear;
		break;
	case TISP_PARAM_GIB_DEIRM_BLC_GR_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_GR_LINEAR);
		tmpbuf = gib->tiziano_gib_deirm_blc_gr_linear;
		break;
	case TISP_PARAM_GIB_DEIRM_BLC_GB_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_GB_LINEAR);
		tmpbuf = gib->tiziano_gib_deirm_blc_gb_linear;
		break;
	case TISP_PARAM_GIB_DEIRM_BLC_B_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_B_LINEAR);
		tmpbuf = gib->tiziano_gib_deirm_blc_b_linear;
		break;
	case TISP_PARAM_GIB_DEIRM_BLC_IR_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_IR_LINEAR);
		tmpbuf = gib->tiziano_gib_deirm_blc_ir_linear;
		break;
	case TISP_PARAM_GIB_IR_POINT:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_IR_POINT);
		tmpbuf = gib->gib_ir_point;
		break;
	case TISP_PARAM_GIB_IR_RESER:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_IR_RESER);
		tmpbuf = gib->gib_ir_reser;
		break;
	case TISP_PARAM_GIB_DEIR_R_H:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_R_H);
		tmpbuf = gib->tiziano_gib_deir_r_h;
		break;
	case TISP_PARAM_GIB_DEIR_G_H:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_G_H);
		tmpbuf = gib->tiziano_gib_deir_g_h;
		break;
	case TISP_PARAM_GIB_DEIR_B_H:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_B_H);
		tmpbuf = gib->tiziano_gib_deir_b_h;
		break;
	case TISP_PARAM_GIB_DEIR_R_M:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_R_M);
		tmpbuf = gib->tiziano_gib_deir_r_m;
		break;
	case TISP_PARAM_GIB_DEIR_G_M:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_G_M);
		tmpbuf = gib->tiziano_gib_deir_g_m;
		break;
	case TISP_PARAM_GIB_DEIR_B_M:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_B_M);
		tmpbuf = gib->tiziano_gib_deir_b_m;
		break;
	case TISP_PARAM_GIB_DEIR_R_L:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_R_L);
		tmpbuf = gib->tiziano_gib_deir_r_l;
		break;
	case TISP_PARAM_GIB_DEIR_G_L:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_G_L);
		tmpbuf = gib->tiziano_gib_deir_g_l;
		break;
	case TISP_PARAM_GIB_DEIR_B_L:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_B_L);
		tmpbuf = gib->tiziano_gib_deir_b_l;
		break;
	case TISP_PARAM_GIB_DEIR_MATRIX_H:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_MATRIX_H);
		tmpbuf = gib->tiziano_gib_deir_matrix_h;
		break;
	case TISP_PARAM_GIB_DEIR_MATRIX_M:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_MATRIX_M);
		tmpbuf = gib->tiziano_gib_deir_matrix_m;
		break;
	case TISP_PARAM_GIB_DEIR_MATRIX_L:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_MATRIX_L);
		tmpbuf = gib->tiziano_gib_deir_matrix_l;
		break;
	default:
		ISP_ERROR("%s,%d: gib not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	memcpy(buf, tmpbuf, len);
	*plen = len;

	return 0;
}

int32_t tisp_gib_param_array_set(tisp_gib_t *gib, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_GIB_CONFIG_LINE:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_CONFIG_LINE);
		tmpbuf = gib->tiziano_gib_config_line;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_R_G_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_R_G_LINEAR);
		tmpbuf = gib->tiziano_gib_r_g_linear;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_B_IR_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_B_IR_LINEAR);
		tmpbuf = gib->tiziano_gib_b_ir_linear;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_DEIRM_BLC_R_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_R_LINEAR);
		tmpbuf = gib->tiziano_gib_deirm_blc_r_linear;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_DEIRM_BLC_GR_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_GR_LINEAR);
		tmpbuf = gib->tiziano_gib_deirm_blc_gr_linear;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_DEIRM_BLC_GB_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_GB_LINEAR);
		tmpbuf = gib->tiziano_gib_deirm_blc_gb_linear;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_DEIRM_BLC_B_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_B_LINEAR);
		tmpbuf = gib->tiziano_gib_deirm_blc_b_linear;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_DEIRM_BLC_IR_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIRM_BLC_IR_LINEAR);
		tmpbuf = gib->tiziano_gib_deirm_blc_ir_linear;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_IR_POINT:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_IR_POINT);
		tmpbuf = gib->gib_ir_point;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_IR_RESER:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_IR_RESER);
		tmpbuf = gib->gib_ir_reser;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_DEIR_R_H:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_R_H);
		tmpbuf = gib->tiziano_gib_deir_r_h;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_DEIR_G_H:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_G_H);
		tmpbuf = gib->tiziano_gib_deir_g_h;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_DEIR_B_H:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_B_H);
		tmpbuf = gib->tiziano_gib_deir_b_h;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_DEIR_R_M:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_R_M);
		tmpbuf = gib->tiziano_gib_deir_r_m;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_DEIR_G_M:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_G_M);
		tmpbuf = gib->tiziano_gib_deir_g_m;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_DEIR_B_M:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_B_M);
		tmpbuf = gib->tiziano_gib_deir_b_m;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_DEIR_R_L:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_R_L);
		tmpbuf = gib->tiziano_gib_deir_r_l;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_DEIR_G_L:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_G_L);
		tmpbuf = gib->tiziano_gib_deir_g_l;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_DEIR_B_L:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_B_L);
		tmpbuf = gib->tiziano_gib_deir_b_l;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_DEIR_MATRIX_H:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_MATRIX_H);
		tmpbuf = gib->tiziano_gib_deir_matrix_h;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_DEIR_MATRIX_M:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_MATRIX_M);
		tmpbuf = gib->tiziano_gib_deir_matrix_m;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_GIB_DEIR_MATRIX_L:
		len = sizeof(tparams.params_data.TISP_PARAM_GIB_DEIR_MATRIX_L);
		tmpbuf = gib->tiziano_gib_deir_matrix_l;
		memcpy(tmpbuf, buf, len);
		break;
	default:
		ISP_ERROR("%s,%d: gib not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	tiziano_gib_lut_parameter(gib);
	gib->trig_set_deir=1;

	return 0;
}
