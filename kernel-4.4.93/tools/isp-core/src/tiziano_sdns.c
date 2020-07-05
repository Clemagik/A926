#include <linux/kernel.h>
#include <linux/irqreturn.h>
#include <linux/dma-mapping.h>

#include "fix_point_calc.h"
#include "../inc/tiziano_sys.h"
#include "tiziano_core.h"
#include "../inc/tiziano_isp.h"
#include "tiziano_sdns.h"
#include "tiziano_params.h"
#include "tiziano_map.h"

/* sdns parameters */
#if 0
int32_t sdns_top_func_array[2];
int32_t sdns_y_dtl_thres_array[9];
int32_t sdns_y_fus_slope_array[9];
int32_t sdns_y_lum_divop_array[9];
int32_t sdns_y_dtl_segop_array[9];
int32_t sdns_y_fus_segop_array[9];
int32_t sdns_y_lum_segop_array[9];
int32_t sdns_y_dsp_segop_array[9];
int32_t sdns_y_bsp_segop_array[9];
int32_t sdns_y_dtl_stren_array[9];
int32_t sdns_y_fus_stren_array[9];
int32_t sdns_y_lum_stren_array[9];
int32_t sdns_y_dsp_stren_array[9];
int32_t sdns_y_bsp_stren_array[9];
int32_t sdns_y_dtl_npv_0_array[9];
int32_t sdns_y_dtl_npv_1_array[9];
int32_t sdns_y_dtl_npv_2_array[9];
int32_t sdns_y_dtl_npv_3_array[9];
int32_t sdns_y_dtl_npv_4_array[9];
int32_t sdns_y_fus_npv_array[16];
int32_t sdns_y_lum_npv_array[16];
int32_t sdns_y_dsp_npv_array[16];
int32_t sdns_y_bsp_npv_array[16];
int32_t sdns_y_bil_stren_array[9];
int32_t sdns_y_bil_npv_array[15];
int32_t sdns_c_bas_wei_array[9];
int32_t sdns_c_fus_mod_array[9];
int32_t sdns_c_flu_cal_array[9];
int32_t sdns_c_flu_stren_array[9];
int32_t sdns_c_flu_npv_array[16];

/* sdns intp parameters */
uint32_t sdns_y_dtl_thres_intp;
uint32_t sdns_y_fus_slope_intp;
uint32_t sdns_y_lum_divop_intp;
uint32_t sdns_y_dtl_segop_intp;
uint32_t sdns_y_fus_segop_intp;
uint32_t sdns_y_lum_segop_intp;
uint32_t sdns_y_dsp_segop_intp;
uint32_t sdns_y_bsp_segop_intp;
uint32_t sdns_y_dtl_stren_intp;
uint32_t sdns_y_fus_stren_intp;
uint32_t sdns_y_lum_stren_intp;
uint32_t sdns_y_dsp_stren_intp;
uint32_t sdns_y_bsp_stren_intp;
uint32_t sdns_y_bil_stren_intp;
uint32_t sdns_y_dtl_npv_0_intp;
uint32_t sdns_y_dtl_npv_1_intp;
uint32_t sdns_y_dtl_npv_2_intp;
uint32_t sdns_y_dtl_npv_3_intp;
uint32_t sdns_y_dtl_npv_4_intp;
uint32_t sdns_c_bas_wei_intp;
uint32_t sdns_c_fus_mod_intp;
uint32_t sdns_c_flu_cal_intp;
uint32_t sdns_c_flu_stren_intp;
#endif
int32_t tisp_sdns_top_func_cfg(tisp_sdns_t *sdns)
{
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_FUN_EN,sdns->sdns_top_func_array[0]);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_UV_FUN_EN,sdns->sdns_top_func_array[1]);

	return 0;
}

uint32_t tisp_sdns_y_param_cfg(tisp_sdns_t *sdns)
{
	uint32_t reg_value;

	uint32_t sdns_y_dtl_value_0 = sdns->sdns_y_dtl_stren_intp*sdns->sdns_y_dtl_npv_0_intp;
	uint32_t sdns_y_dtl_value_1 = sdns->sdns_y_dtl_stren_intp*sdns->sdns_y_dtl_npv_1_intp;
	uint32_t sdns_y_dtl_value_2 = sdns->sdns_y_dtl_stren_intp*sdns->sdns_y_dtl_npv_2_intp;
	uint32_t sdns_y_dtl_value_3 = sdns->sdns_y_dtl_stren_intp*sdns->sdns_y_dtl_npv_3_intp;
	uint32_t sdns_y_dtl_value_4 = sdns->sdns_y_dtl_stren_intp*sdns->sdns_y_dtl_npv_4_intp;
	uint32_t sdns_y_dtl_value_5 = sdns->sdns_y_dtl_stren_intp*sdns->sdns_y_dtl_npv_4_intp;
	uint32_t sdns_y_dtl_value_6 = sdns->sdns_y_dtl_stren_intp*sdns->sdns_y_dtl_npv_4_intp;
	uint32_t sdns_y_dtl_value_7 = sdns->sdns_y_dtl_stren_intp*sdns->sdns_y_dtl_npv_4_intp;
	uint32_t sdns_y_dtl_value_8 = sdns->sdns_y_dtl_stren_intp*sdns->sdns_y_dtl_npv_4_intp;
	uint32_t sdns_y_dtl_value_9 = sdns->sdns_y_dtl_stren_intp*sdns->sdns_y_dtl_npv_4_intp;
	uint32_t sdns_y_dtl_value_10 = sdns->sdns_y_dtl_stren_intp*sdns->sdns_y_dtl_npv_4_intp;
	uint32_t sdns_y_dtl_value_11 = sdns->sdns_y_dtl_stren_intp*sdns->sdns_y_dtl_npv_4_intp;
	uint32_t sdns_y_dtl_value_12 = sdns->sdns_y_dtl_stren_intp*sdns->sdns_y_dtl_npv_4_intp;
	uint32_t sdns_y_dtl_value_13 = sdns->sdns_y_dtl_stren_intp*sdns->sdns_y_dtl_npv_4_intp;
	uint32_t sdns_y_dtl_value_14 = sdns->sdns_y_dtl_stren_intp*sdns->sdns_y_dtl_npv_4_intp;
	uint32_t sdns_y_dtl_value_15 = sdns->sdns_y_dtl_stren_intp*sdns->sdns_y_dtl_npv_4_intp;

	uint32_t sdns_y_fus_value_0 = sdns->sdns_y_fus_stren_intp*sdns->sdns_y_fus_npv_array[0];
	uint32_t sdns_y_fus_value_1 = sdns->sdns_y_fus_stren_intp*sdns->sdns_y_fus_npv_array[1];
	uint32_t sdns_y_fus_value_2 = sdns->sdns_y_fus_stren_intp*sdns->sdns_y_fus_npv_array[2];
	uint32_t sdns_y_fus_value_3 = sdns->sdns_y_fus_stren_intp*sdns->sdns_y_fus_npv_array[3];
	uint32_t sdns_y_fus_value_4 = sdns->sdns_y_fus_stren_intp*sdns->sdns_y_fus_npv_array[4];
	uint32_t sdns_y_fus_value_5 = sdns->sdns_y_fus_stren_intp*sdns->sdns_y_fus_npv_array[5];
	uint32_t sdns_y_fus_value_6 = sdns->sdns_y_fus_stren_intp*sdns->sdns_y_fus_npv_array[6];
	uint32_t sdns_y_fus_value_7 = sdns->sdns_y_fus_stren_intp*sdns->sdns_y_fus_npv_array[7];
	uint32_t sdns_y_fus_value_8 = sdns->sdns_y_fus_stren_intp*sdns->sdns_y_fus_npv_array[8];
	uint32_t sdns_y_fus_value_9 = sdns->sdns_y_fus_stren_intp*sdns->sdns_y_fus_npv_array[9];
	uint32_t sdns_y_fus_value_10 = sdns->sdns_y_fus_stren_intp*sdns->sdns_y_fus_npv_array[10];
	uint32_t sdns_y_fus_value_11 = sdns->sdns_y_fus_stren_intp*sdns->sdns_y_fus_npv_array[11];
	uint32_t sdns_y_fus_value_12 = sdns->sdns_y_fus_stren_intp*sdns->sdns_y_fus_npv_array[12];
	uint32_t sdns_y_fus_value_13 = sdns->sdns_y_fus_stren_intp*sdns->sdns_y_fus_npv_array[13];
	uint32_t sdns_y_fus_value_14 = sdns->sdns_y_fus_stren_intp*sdns->sdns_y_fus_npv_array[14];
	uint32_t sdns_y_fus_value_15 = sdns->sdns_y_fus_stren_intp*sdns->sdns_y_fus_npv_array[15];

	uint32_t sdns_y_lum_value_0 = sdns->sdns_y_lum_stren_intp*sdns->sdns_y_lum_npv_array[0];
	uint32_t sdns_y_lum_value_1 = sdns->sdns_y_lum_stren_intp*sdns->sdns_y_lum_npv_array[1];
	uint32_t sdns_y_lum_value_2 = sdns->sdns_y_lum_stren_intp*sdns->sdns_y_lum_npv_array[2];
	uint32_t sdns_y_lum_value_3 = sdns->sdns_y_lum_stren_intp*sdns->sdns_y_lum_npv_array[3];
	uint32_t sdns_y_lum_value_4 = sdns->sdns_y_lum_stren_intp*sdns->sdns_y_lum_npv_array[4];
	uint32_t sdns_y_lum_value_5 = sdns->sdns_y_lum_stren_intp*sdns->sdns_y_lum_npv_array[5];
	uint32_t sdns_y_lum_value_6 = sdns->sdns_y_lum_stren_intp*sdns->sdns_y_lum_npv_array[6];
	uint32_t sdns_y_lum_value_7 = sdns->sdns_y_lum_stren_intp*sdns->sdns_y_lum_npv_array[7];
	uint32_t sdns_y_lum_value_8 = sdns->sdns_y_lum_stren_intp*sdns->sdns_y_lum_npv_array[8];
	uint32_t sdns_y_lum_value_9 = sdns->sdns_y_lum_stren_intp*sdns->sdns_y_lum_npv_array[9];
	uint32_t sdns_y_lum_value_10 = sdns->sdns_y_lum_stren_intp*sdns->sdns_y_lum_npv_array[10];
	uint32_t sdns_y_lum_value_11 = sdns->sdns_y_lum_stren_intp*sdns->sdns_y_lum_npv_array[11];
	uint32_t sdns_y_lum_value_12 = sdns->sdns_y_lum_stren_intp*sdns->sdns_y_lum_npv_array[12];
	uint32_t sdns_y_lum_value_13 = sdns->sdns_y_lum_stren_intp*sdns->sdns_y_lum_npv_array[13];
	uint32_t sdns_y_lum_value_14 = sdns->sdns_y_lum_stren_intp*sdns->sdns_y_lum_npv_array[14];
	uint32_t sdns_y_lum_value_15 = sdns->sdns_y_lum_stren_intp*sdns->sdns_y_lum_npv_array[15];

	uint32_t sdns_y_bsp_value_0 = sdns->sdns_y_bsp_stren_intp*sdns->sdns_y_bsp_npv_array[0];
	uint32_t sdns_y_bsp_value_1 = sdns->sdns_y_bsp_stren_intp*sdns->sdns_y_bsp_npv_array[1];
	uint32_t sdns_y_bsp_value_2 = sdns->sdns_y_bsp_stren_intp*sdns->sdns_y_bsp_npv_array[2];
	uint32_t sdns_y_bsp_value_3 = sdns->sdns_y_bsp_stren_intp*sdns->sdns_y_bsp_npv_array[3];
	uint32_t sdns_y_bsp_value_4 = sdns->sdns_y_bsp_stren_intp*sdns->sdns_y_bsp_npv_array[4];
	uint32_t sdns_y_bsp_value_5 = sdns->sdns_y_bsp_stren_intp*sdns->sdns_y_bsp_npv_array[5];
	uint32_t sdns_y_bsp_value_6 = sdns->sdns_y_bsp_stren_intp*sdns->sdns_y_bsp_npv_array[6];
	uint32_t sdns_y_bsp_value_7 = sdns->sdns_y_bsp_stren_intp*sdns->sdns_y_bsp_npv_array[7];
	uint32_t sdns_y_bsp_value_8 = sdns->sdns_y_bsp_stren_intp*sdns->sdns_y_bsp_npv_array[8];
	uint32_t sdns_y_bsp_value_9 = sdns->sdns_y_bsp_stren_intp*sdns->sdns_y_bsp_npv_array[9];
	uint32_t sdns_y_bsp_value_10 = sdns->sdns_y_bsp_stren_intp*sdns->sdns_y_bsp_npv_array[10];
	uint32_t sdns_y_bsp_value_11 = sdns->sdns_y_bsp_stren_intp*sdns->sdns_y_bsp_npv_array[11];
	uint32_t sdns_y_bsp_value_12 = sdns->sdns_y_bsp_stren_intp*sdns->sdns_y_bsp_npv_array[12];
	uint32_t sdns_y_bsp_value_13 = sdns->sdns_y_bsp_stren_intp*sdns->sdns_y_bsp_npv_array[13];
	uint32_t sdns_y_bsp_value_14 = sdns->sdns_y_bsp_stren_intp*sdns->sdns_y_bsp_npv_array[14];
	uint32_t sdns_y_bsp_value_15 = sdns->sdns_y_bsp_stren_intp*sdns->sdns_y_bsp_npv_array[15];

	uint32_t sdns_y_dsp_value_0 = sdns->sdns_y_dsp_stren_intp*sdns->sdns_y_dsp_npv_array[0];
	uint32_t sdns_y_dsp_value_1 = sdns->sdns_y_dsp_stren_intp*sdns->sdns_y_dsp_npv_array[1];
	uint32_t sdns_y_dsp_value_2 = sdns->sdns_y_dsp_stren_intp*sdns->sdns_y_dsp_npv_array[2];
	uint32_t sdns_y_dsp_value_3 = sdns->sdns_y_dsp_stren_intp*sdns->sdns_y_dsp_npv_array[3];
	uint32_t sdns_y_dsp_value_4 = sdns->sdns_y_dsp_stren_intp*sdns->sdns_y_dsp_npv_array[4];
	uint32_t sdns_y_dsp_value_5 = sdns->sdns_y_dsp_stren_intp*sdns->sdns_y_dsp_npv_array[5];
	uint32_t sdns_y_dsp_value_6 = sdns->sdns_y_dsp_stren_intp*sdns->sdns_y_dsp_npv_array[6];
	uint32_t sdns_y_dsp_value_7 = sdns->sdns_y_dsp_stren_intp*sdns->sdns_y_dsp_npv_array[7];
	uint32_t sdns_y_dsp_value_8 = sdns->sdns_y_dsp_stren_intp*sdns->sdns_y_dsp_npv_array[8];
	uint32_t sdns_y_dsp_value_9 = sdns->sdns_y_dsp_stren_intp*sdns->sdns_y_dsp_npv_array[9];
	uint32_t sdns_y_dsp_value_10 = sdns->sdns_y_dsp_stren_intp*sdns->sdns_y_dsp_npv_array[10];
	uint32_t sdns_y_dsp_value_11 = sdns->sdns_y_dsp_stren_intp*sdns->sdns_y_dsp_npv_array[11];
	uint32_t sdns_y_dsp_value_12 = sdns->sdns_y_dsp_stren_intp*sdns->sdns_y_dsp_npv_array[12];
	uint32_t sdns_y_dsp_value_13 = sdns->sdns_y_dsp_stren_intp*sdns->sdns_y_dsp_npv_array[13];
	uint32_t sdns_y_dsp_value_14 = sdns->sdns_y_dsp_stren_intp*sdns->sdns_y_dsp_npv_array[14];
	uint32_t sdns_y_dsp_value_15 = sdns->sdns_y_dsp_stren_intp*sdns->sdns_y_dsp_npv_array[15];

	uint32_t sdns_y_bil_value_0 = sdns->sdns_y_bil_stren_intp*sdns->sdns_y_bil_npv_array[0];
	uint32_t sdns_y_bil_value_1 = sdns->sdns_y_bil_stren_intp*sdns->sdns_y_bil_npv_array[1];
	uint32_t sdns_y_bil_value_2 = sdns->sdns_y_bil_stren_intp*sdns->sdns_y_bil_npv_array[2];
	uint32_t sdns_y_bil_value_3 = sdns->sdns_y_bil_stren_intp*sdns->sdns_y_bil_npv_array[3];
	uint32_t sdns_y_bil_value_4 = sdns->sdns_y_bil_stren_intp*sdns->sdns_y_bil_npv_array[4];
	uint32_t sdns_y_bil_value_5 = sdns->sdns_y_bil_stren_intp*sdns->sdns_y_bil_npv_array[5];
	uint32_t sdns_y_bil_value_6 = sdns->sdns_y_bil_stren_intp*sdns->sdns_y_bil_npv_array[6];
	uint32_t sdns_y_bil_value_7 = sdns->sdns_y_bil_stren_intp*sdns->sdns_y_bil_npv_array[7];
	uint32_t sdns_y_bil_value_8 = sdns->sdns_y_bil_stren_intp*sdns->sdns_y_bil_npv_array[8];
	uint32_t sdns_y_bil_value_9 = sdns->sdns_y_bil_stren_intp*sdns->sdns_y_bil_npv_array[9];
	uint32_t sdns_y_bil_value_10 = sdns->sdns_y_bil_stren_intp*sdns->sdns_y_bil_npv_array[10];
	uint32_t sdns_y_bil_value_11 = sdns->sdns_y_bil_stren_intp*sdns->sdns_y_bil_npv_array[11];
	uint32_t sdns_y_bil_value_12 = sdns->sdns_y_bil_stren_intp*sdns->sdns_y_bil_npv_array[12];
	uint32_t sdns_y_bil_value_13 = sdns->sdns_y_bil_stren_intp*sdns->sdns_y_bil_npv_array[13];
	uint32_t sdns_y_bil_value_14 = sdns->sdns_y_bil_stren_intp*sdns->sdns_y_bil_npv_array[14];

	reg_value = (sdns->sdns_y_fus_slope_intp<< 16) |
		(sdns->sdns_y_dtl_thres_intp<< 8) |
		(sdns->sdns_y_lum_divop_intp);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_COM_PAR0,reg_value);

	reg_value = (sdns->sdns_y_dsp_segop_intp<< 28) |
		(sdns->sdns_y_bsp_segop_intp<< 24) |
		(sdns->sdns_y_lum_segop_intp<< 16) |
		(sdns->sdns_y_dtl_segop_intp<< 8) |
		(sdns->sdns_y_fus_segop_intp);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_COM_PAR1,reg_value);

	reg_value = (sdns_y_dtl_value_3 << 24) |
		(sdns_y_dtl_value_2 << 16) |
		(sdns_y_dtl_value_1 << 8) |
		(sdns_y_dtl_value_0);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_DETAIL_PAR0,reg_value);
	reg_value = (sdns_y_dtl_value_7 << 24) |
		(sdns_y_dtl_value_6 << 16) |
		(sdns_y_dtl_value_5 << 8) |
		(sdns_y_dtl_value_4);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_DETAIL_PAR1,reg_value);
	reg_value = (sdns_y_dtl_value_11<< 24) |
		(sdns_y_dtl_value_10<< 16) |
		(sdns_y_dtl_value_9 << 8) |
		(sdns_y_dtl_value_8);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_DETAIL_PAR2,reg_value);
	reg_value = (sdns_y_dtl_value_15<< 24) |
		(sdns_y_dtl_value_14<< 16) |
		(sdns_y_dtl_value_13<< 8) |
		(sdns_y_dtl_value_12);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_DETAIL_PAR3,reg_value);

	reg_value = (sdns_y_fus_value_3 << 24) |
		(sdns_y_fus_value_2 << 16) |
		(sdns_y_fus_value_1 << 8) |
		(sdns_y_fus_value_0);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_FUSION_PAR0,reg_value);
	reg_value = (sdns_y_fus_value_7 << 24) |
		(sdns_y_fus_value_6 << 16) |
		(sdns_y_fus_value_5 << 8) |
		(sdns_y_fus_value_4);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_FUSION_PAR1,reg_value);
	reg_value = (sdns_y_fus_value_11<< 24) |
		(sdns_y_fus_value_10<< 16) |
		(sdns_y_fus_value_9 << 8) |
		(sdns_y_fus_value_8);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_FUSION_PAR2,reg_value);
	reg_value = (sdns_y_fus_value_15<< 24) |
		(sdns_y_fus_value_14<< 16) |
		(sdns_y_fus_value_13<< 8) |
		(sdns_y_fus_value_12);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_FUSION_PAR3,reg_value);

	reg_value = (sdns_y_lum_value_3 << 24) |
		(sdns_y_lum_value_2 << 16) |
		(sdns_y_lum_value_1 << 8) |
		(sdns_y_lum_value_0);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_LUMA_PAR0,reg_value);
	reg_value = (sdns_y_lum_value_7 << 24) |
		(sdns_y_lum_value_6 << 16) |
		(sdns_y_lum_value_5 << 8) |
		(sdns_y_lum_value_4);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_LUMA_PAR1,reg_value);
	reg_value = (sdns_y_lum_value_11<< 24) |
		(sdns_y_lum_value_10<< 16) |
		(sdns_y_lum_value_9 << 8) |
		(sdns_y_lum_value_8);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_LUMA_PAR2,reg_value);
	reg_value = (sdns_y_lum_value_15<< 24) |
		(sdns_y_lum_value_14<< 16) |
		(sdns_y_lum_value_13<< 8) |
		(sdns_y_lum_value_12);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_LUMA_PAR3,reg_value);

	reg_value = (sdns_y_bsp_value_3 << 24) |
		(sdns_y_bsp_value_2 << 16) |
		(sdns_y_bsp_value_1 << 8) |
		(sdns_y_bsp_value_0);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_BSHP_PAR0,reg_value);
	reg_value = (sdns_y_bsp_value_7 << 24) |
		(sdns_y_bsp_value_6 << 16) |
		(sdns_y_bsp_value_5 << 8) |
		(sdns_y_bsp_value_4);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_BSHP_PAR1,reg_value);
	reg_value = (sdns_y_bsp_value_11<< 24) |
		(sdns_y_bsp_value_10<< 16) |
		(sdns_y_bsp_value_9 << 8) |
		(sdns_y_bsp_value_8);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_BSHP_PAR2,reg_value);
	reg_value = (sdns_y_bsp_value_15<< 24) |
		(sdns_y_bsp_value_14<< 16) |
		(sdns_y_bsp_value_13<< 8) |
		(sdns_y_bsp_value_12);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_BSHP_PAR3,reg_value);

	reg_value = (sdns_y_dsp_value_3 << 24) |
		(sdns_y_dsp_value_2 << 16) |
		(sdns_y_dsp_value_1 << 8) |
		(sdns_y_dsp_value_0);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_DSHP_PAR0,reg_value);
	reg_value = (sdns_y_dsp_value_7 << 24) |
		(sdns_y_dsp_value_6 << 16) |
		(sdns_y_dsp_value_5 << 8) |
		(sdns_y_dsp_value_4);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_DSHP_PAR1,reg_value);
	reg_value = (sdns_y_dsp_value_11<< 24) |
		(sdns_y_dsp_value_10<< 16) |
		(sdns_y_dsp_value_9 << 8) |
		(sdns_y_dsp_value_8);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_DSHP_PAR2,reg_value);
	reg_value = (sdns_y_dsp_value_15<< 24) |
		(sdns_y_dsp_value_14<< 16) |
		(sdns_y_dsp_value_13<< 8) |
		(sdns_y_dsp_value_12);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_DSHP_PAR3,reg_value);

	reg_value = (sdns_y_bil_value_3 << 24) |
		(sdns_y_bil_value_2 << 16) |
		(sdns_y_bil_value_1 << 8) |
		(sdns_y_bil_value_0);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_BI_PAR0,reg_value);
	reg_value = (sdns_y_bil_value_7 << 24) |
		(sdns_y_bil_value_6 << 16) |
		(sdns_y_bil_value_5 << 8) |
		(sdns_y_bil_value_4);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_BI_PAR1,reg_value);
	reg_value = (sdns_y_bil_value_11<< 24) |
		(sdns_y_bil_value_10<< 16) |
		(sdns_y_bil_value_9 << 8) |
		(sdns_y_bil_value_8);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_BI_PAR2,reg_value);
	reg_value = (sdns_y_bil_value_14<< 16) |
		(sdns_y_bil_value_13<< 8) |
		(sdns_y_bil_value_12);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_Y_BI_PAR3,reg_value);

	return 0;
}

int32_t tisp_sdns_c_param_cfg(tisp_sdns_t *sdns){
	uint32_t reg_value;

	uint32_t sdns_c_fluct_value_0 = sdns->sdns_c_flu_stren_intp*sdns->sdns_c_flu_npv_array[0];
	uint32_t sdns_c_fluct_value_1 = sdns->sdns_c_flu_stren_intp*sdns->sdns_c_flu_npv_array[1];
	uint32_t sdns_c_fluct_value_2 = sdns->sdns_c_flu_stren_intp*sdns->sdns_c_flu_npv_array[2];
	uint32_t sdns_c_fluct_value_3 = sdns->sdns_c_flu_stren_intp*sdns->sdns_c_flu_npv_array[3];
	uint32_t sdns_c_fluct_value_4 = sdns->sdns_c_flu_stren_intp*sdns->sdns_c_flu_npv_array[4];
	uint32_t sdns_c_fluct_value_5 = sdns->sdns_c_flu_stren_intp*sdns->sdns_c_flu_npv_array[5];
	uint32_t sdns_c_fluct_value_6 = sdns->sdns_c_flu_stren_intp*sdns->sdns_c_flu_npv_array[6];
	uint32_t sdns_c_fluct_value_7 = sdns->sdns_c_flu_stren_intp*sdns->sdns_c_flu_npv_array[7];
	uint32_t sdns_c_fluct_value_8 = sdns->sdns_c_flu_stren_intp*sdns->sdns_c_flu_npv_array[8];
	uint32_t sdns_c_fluct_value_9 = sdns->sdns_c_flu_stren_intp*sdns->sdns_c_flu_npv_array[9];
	uint32_t sdns_c_fluct_value_10 = sdns->sdns_c_flu_stren_intp*sdns->sdns_c_flu_npv_array[10];
	uint32_t sdns_c_fluct_value_11 = sdns->sdns_c_flu_stren_intp*sdns->sdns_c_flu_npv_array[11];
	uint32_t sdns_c_fluct_value_12 = sdns->sdns_c_flu_stren_intp*sdns->sdns_c_flu_npv_array[12];
	uint32_t sdns_c_fluct_value_13 = sdns->sdns_c_flu_stren_intp*sdns->sdns_c_flu_npv_array[13];
	uint32_t sdns_c_fluct_value_14 = sdns->sdns_c_flu_stren_intp*sdns->sdns_c_flu_npv_array[14];
	uint32_t sdns_c_fluct_value_15 = sdns->sdns_c_flu_stren_intp*sdns->sdns_c_flu_npv_array[15];

	reg_value = (sdns->sdns_c_bas_wei_intp << 16) |
		(sdns->sdns_c_fus_mod_intp << 8) |
		(sdns->sdns_c_flu_cal_intp);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_UV_COM_PAR, reg_value);

	reg_value = (sdns_c_fluct_value_3 << 24) |
		(sdns_c_fluct_value_2 << 16) |
		(sdns_c_fluct_value_1 << 8) |
		(sdns_c_fluct_value_0);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_UV_FUSION_PAR0, reg_value);
	reg_value = (sdns_c_fluct_value_7 << 24) |
		(sdns_c_fluct_value_6 << 16) |
		(sdns_c_fluct_value_5 << 8) |
		(sdns_c_fluct_value_4);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_UV_FUSION_PAR1, reg_value);
	reg_value = (sdns_c_fluct_value_11<< 24) |
		(sdns_c_fluct_value_10<< 16) |
		(sdns_c_fluct_value_9 << 8) |
		(sdns_c_fluct_value_8);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_UV_FUSION_PAR2, reg_value);
	reg_value = (sdns_c_fluct_value_15<< 24) |
		(sdns_c_fluct_value_14<< 16) |
		(sdns_c_fluct_value_13<< 8) |
		(sdns_c_fluct_value_12);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_UV_FUSION_PAR3, reg_value);

	return 0;
}

int32_t tisp_sdns_intp(tisp_sdns_t *sdns, int32_t gain_value)
{
	int32_t x_int, x_fra;

	x_int = gain_value >> 16;
	x_fra = gain_value & 0xffff;

	sdns->sdns_y_dtl_thres_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_y_dtl_thres_array);
	sdns->sdns_y_fus_slope_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_y_fus_slope_array);
	sdns->sdns_y_lum_divop_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_y_lum_divop_array);
	sdns->sdns_y_dtl_segop_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_y_dtl_segop_array);
	sdns->sdns_y_fus_segop_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_y_fus_segop_array);
	sdns->sdns_y_lum_segop_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_y_lum_segop_array);
	sdns->sdns_y_dsp_segop_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_y_dsp_segop_array);
	sdns->sdns_y_bsp_segop_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_y_bsp_segop_array);
	sdns->sdns_y_dtl_stren_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_y_dtl_stren_array);
	sdns->sdns_y_fus_stren_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_y_fus_stren_array);
	sdns->sdns_y_lum_stren_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_y_lum_stren_array);
	sdns->sdns_y_dsp_stren_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_y_dsp_stren_array);
	sdns->sdns_y_bsp_stren_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_y_bsp_stren_array);
	sdns->sdns_y_bil_stren_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_y_bil_stren_array);
	sdns->sdns_y_dtl_npv_0_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_y_dtl_npv_0_array);
	sdns->sdns_y_dtl_npv_1_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_y_dtl_npv_1_array);
	sdns->sdns_y_dtl_npv_2_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_y_dtl_npv_2_array);
	sdns->sdns_y_dtl_npv_3_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_y_dtl_npv_3_array);
	sdns->sdns_y_dtl_npv_4_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_y_dtl_npv_4_array);
	sdns->sdns_c_bas_wei_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_c_bas_wei_array);
	sdns->sdns_c_fus_mod_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_c_fus_mod_array);
	sdns->sdns_c_flu_cal_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_c_flu_cal_array);
	sdns->sdns_c_flu_stren_intp = tisp_simple_intp(x_int,x_fra,sdns->sdns_c_flu_stren_array);
	return 0;
}

int32_t tisp_sdns_all_reg_refresh(tisp_sdns_t *sdns, int gain_value)
{
	tisp_sdns_intp(sdns, gain_value);

	tisp_sdns_y_param_cfg(sdns);
	tisp_sdns_c_param_cfg(sdns);
	tisp_sdns_top_func_cfg(sdns);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_REG_CTRL,0x11);

	return 0;
}

int32_t tisp_sdns_intp_reg_refresh(tisp_sdns_t *sdns, int gain_value)
{
	tisp_sdns_intp(sdns, gain_value);

	tisp_sdns_y_param_cfg(sdns);
	tisp_sdns_c_param_cfg(sdns);
	tisp_sdns_top_func_cfg(sdns);
	system_reg_write(((tisp_core_t *)sdns->core)->priv_data, SDNS_ADDR_REG_CTRL,0x11);

	return 0;
}


uint32_t tisp_sdns_par_refresh(tisp_sdns_t *sdns, uint32_t gain_value, uint32_t gain_thres)
{
	uint32_t gain_diff;

	if(sdns->gain_old == 0xffffffff){
		sdns->gain_old = gain_value;
		tisp_sdns_all_reg_refresh(sdns, gain_value);
	} else {
		if (gain_value >= sdns->gain_old)
			gain_diff = gain_value-sdns->gain_old;
		else
			gain_diff = sdns->gain_old-gain_value;

		if (gain_diff >= gain_thres) {
			sdns->gain_old = gain_value;
			tisp_sdns_intp_reg_refresh(sdns, gain_value);
		}
	}

	return 0;
}

int32_t tiziano_sdns_params_refresh(tisp_sdns_t *sdns)
{

	memcpy(sdns->sdns_top_func_array, tparams.params_data.TISP_PARAM_SDNS_TOP_FUNC_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_TOP_FUNC_ARRAY));
	memcpy(sdns->sdns_y_dtl_thres_array, tparams.params_data.TISP_PARAM_SDNS_Y_DTL_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_THRES_ARRAY));
	memcpy(sdns->sdns_y_fus_slope_array, tparams.params_data.TISP_PARAM_SDNS_Y_FUS_SLOPE_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_FUS_SLOPE_ARRAY));
	memcpy(sdns->sdns_y_lum_divop_array, tparams.params_data.TISP_PARAM_SDNS_Y_LUM_DIVOP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_LUM_DIVOP_ARRAY));
	memcpy(sdns->sdns_y_dtl_segop_array, tparams.params_data.TISP_PARAM_SDNS_Y_DTL_SEGOP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_SEGOP_ARRAY));
	memcpy(sdns->sdns_y_fus_segop_array, tparams.params_data.TISP_PARAM_SDNS_Y_FUS_SEGOP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_FUS_SEGOP_ARRAY));
	memcpy(sdns->sdns_y_lum_segop_array, tparams.params_data.TISP_PARAM_SDNS_Y_LUM_SEGOP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_LUM_SEGOP_ARRAY));
	memcpy(sdns->sdns_y_dsp_segop_array, tparams.params_data.TISP_PARAM_SDNS_Y_DSP_SEGOP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DSP_SEGOP_ARRAY));
	memcpy(sdns->sdns_y_bsp_segop_array, tparams.params_data.TISP_PARAM_SDNS_Y_BSP_SEGOP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_BSP_SEGOP_ARRAY));
	memcpy(sdns->sdns_y_dtl_stren_array, tparams.params_data.TISP_PARAM_SDNS_Y_DTL_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_STREN_ARRAY));
	memcpy(sdns->sdns_y_fus_stren_array, tparams.params_data.TISP_PARAM_SDNS_Y_FUS_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_FUS_STREN_ARRAY));
	memcpy(sdns->sdns_y_lum_stren_array, tparams.params_data.TISP_PARAM_SDNS_Y_LUM_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_LUM_STREN_ARRAY));
	memcpy(sdns->sdns_y_dsp_stren_array, tparams.params_data.TISP_PARAM_SDNS_Y_DSP_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DSP_STREN_ARRAY));
	memcpy(sdns->sdns_y_bsp_stren_array, tparams.params_data.TISP_PARAM_SDNS_Y_BSP_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_BSP_STREN_ARRAY));
	memcpy(sdns->sdns_y_dtl_npv_0_array, tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_0_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_0_ARRAY));
	memcpy(sdns->sdns_y_dtl_npv_1_array, tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_1_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_1_ARRAY));
	memcpy(sdns->sdns_y_dtl_npv_2_array, tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_2_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_2_ARRAY));
	memcpy(sdns->sdns_y_dtl_npv_3_array, tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_3_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_3_ARRAY));
	memcpy(sdns->sdns_y_dtl_npv_4_array, tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_4_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_4_ARRAY));
	memcpy(sdns->sdns_y_fus_npv_array, tparams.params_data.TISP_PARAM_SDNS_Y_FUS_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_FUS_NPV_ARRAY));
	memcpy(sdns->sdns_y_lum_npv_array, tparams.params_data.TISP_PARAM_SDNS_Y_LUM_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_LUM_NPV_ARRAY));
	memcpy(sdns->sdns_y_dsp_npv_array, tparams.params_data.TISP_PARAM_SDNS_Y_DSP_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DSP_NPV_ARRAY));
	memcpy(sdns->sdns_y_bsp_npv_array, tparams.params_data.TISP_PARAM_SDNS_Y_BSP_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_BSP_NPV_ARRAY));
	memcpy(sdns->sdns_y_bil_stren_array, tparams.params_data.TISP_PARAM_SDNS_Y_BIL_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_BIL_STREN_ARRAY));
	memcpy(sdns->sdns_y_bil_npv_array, tparams.params_data.TISP_PARAM_SDNS_Y_BIL_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_BIL_NPV_ARRAY));
	memcpy(sdns->sdns_c_bas_wei_array, tparams.params_data.TISP_PARAM_SDNS_C_BAS_WEI_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_C_BAS_WEI_ARRAY));
	memcpy(sdns->sdns_c_fus_mod_array, tparams.params_data.TISP_PARAM_SDNS_C_FUS_MOD_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_C_FUS_MOD_ARRAY));
	memcpy(sdns->sdns_c_flu_cal_array, tparams.params_data.TISP_PARAM_SDNS_C_FLU_CAL_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_C_FLU_CAL_ARRAY));
	memcpy(sdns->sdns_c_flu_stren_array, tparams.params_data.TISP_PARAM_SDNS_C_FLU_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_C_FLU_STREN_ARRAY));
	memcpy(sdns->sdns_c_flu_npv_array, tparams.params_data.TISP_PARAM_SDNS_C_FLU_NPV_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SDNS_C_FLU_NPV_ARRAY));

	return 0;
}

int32_t tiziano_sdns_dn_params_refresh(tisp_sdns_t *sdns)
{
	sdns->gain_old += 0x200;
	tiziano_sdns_params_refresh(sdns);
	tisp_sdns_all_reg_refresh(sdns, sdns->gain_old);

	return 0;
}

int32_t tiziano_sdns_init(tisp_sdns_t *sdns)
{
	sdns->gain_old = 0xffffffff;
	sdns->gain_thres = 0x100;
	tiziano_sdns_params_refresh(sdns);
	tisp_sdns_par_refresh(sdns, 0x10000, 0x10000);

	return 0;
}

int32_t tisp_sdns_refresh(tisp_sdns_t *sdns, uint32_t gain_value)
{
	tisp_sdns_par_refresh(sdns, gain_value, sdns->gain_thres);

	return 0;
}

int32_t tisp_sdns_param_array_get(tisp_sdns_t *sdns, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_SDNS_TOP_FUNC_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_TOP_FUNC_ARRAY);
		tmpbuf = sdns->sdns_top_func_array;
		break;
	case TISP_PARAM_SDNS_Y_DTL_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_THRES_ARRAY);
		tmpbuf = sdns->sdns_y_dtl_thres_array;
		break;
	case TISP_PARAM_SDNS_Y_FUS_SLOPE_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_FUS_SLOPE_ARRAY);
		tmpbuf = sdns->sdns_y_fus_slope_array;
		break;
	case TISP_PARAM_SDNS_Y_LUM_DIVOP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_LUM_DIVOP_ARRAY);
		tmpbuf = sdns->sdns_y_lum_divop_array;
		break;
	case TISP_PARAM_SDNS_Y_DTL_SEGOP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_SEGOP_ARRAY);
		tmpbuf = sdns->sdns_y_dtl_segop_array;
		break;
	case TISP_PARAM_SDNS_Y_FUS_SEGOP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_FUS_SEGOP_ARRAY);
		tmpbuf = sdns->sdns_y_fus_segop_array;
		break;
	case TISP_PARAM_SDNS_Y_LUM_SEGOP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_LUM_SEGOP_ARRAY);
		tmpbuf = sdns->sdns_y_lum_segop_array;
		break;
	case TISP_PARAM_SDNS_Y_DSP_SEGOP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DSP_SEGOP_ARRAY);
		tmpbuf = sdns->sdns_y_dsp_segop_array;
		break;
	case TISP_PARAM_SDNS_Y_BSP_SEGOP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_BSP_SEGOP_ARRAY);
		tmpbuf = sdns->sdns_y_bsp_segop_array;
		break;
	case TISP_PARAM_SDNS_Y_DTL_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_STREN_ARRAY);
		tmpbuf = sdns->sdns_y_dtl_stren_array;
		break;
	case TISP_PARAM_SDNS_Y_FUS_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_FUS_STREN_ARRAY);
		tmpbuf = sdns->sdns_y_fus_stren_array;
		break;
	case TISP_PARAM_SDNS_Y_LUM_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_LUM_STREN_ARRAY);
		tmpbuf = sdns->sdns_y_lum_stren_array;
		break;
	case TISP_PARAM_SDNS_Y_DSP_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DSP_STREN_ARRAY);
		tmpbuf = sdns->sdns_y_dsp_stren_array;
		break;
	case TISP_PARAM_SDNS_Y_BSP_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_BSP_STREN_ARRAY);
		tmpbuf = sdns->sdns_y_bsp_stren_array;
		break;
	case TISP_PARAM_SDNS_Y_DTL_NPV_0_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_0_ARRAY);
		tmpbuf = sdns->sdns_y_dtl_npv_0_array;
		break;
	case TISP_PARAM_SDNS_Y_DTL_NPV_1_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_1_ARRAY);
		tmpbuf = sdns->sdns_y_dtl_npv_1_array;
		break;
	case TISP_PARAM_SDNS_Y_DTL_NPV_2_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_2_ARRAY);
		tmpbuf = sdns->sdns_y_dtl_npv_2_array;
		break;
	case TISP_PARAM_SDNS_Y_DTL_NPV_3_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_3_ARRAY);
		tmpbuf = sdns->sdns_y_dtl_npv_3_array;
		break;
	case TISP_PARAM_SDNS_Y_DTL_NPV_4_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_4_ARRAY);
		tmpbuf = sdns->sdns_y_dtl_npv_4_array;
		break;
	case TISP_PARAM_SDNS_Y_FUS_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_FUS_NPV_ARRAY);
		tmpbuf = sdns->sdns_y_fus_npv_array;
		break;
	case TISP_PARAM_SDNS_Y_LUM_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_LUM_NPV_ARRAY);
		tmpbuf = sdns->sdns_y_lum_npv_array;
		break;
	case TISP_PARAM_SDNS_Y_DSP_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DSP_NPV_ARRAY);
		tmpbuf = sdns->sdns_y_dsp_npv_array;
		break;
	case TISP_PARAM_SDNS_Y_BSP_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_BSP_NPV_ARRAY);
		tmpbuf = sdns->sdns_y_bsp_npv_array;
		break;
	case TISP_PARAM_SDNS_Y_BIL_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_BIL_STREN_ARRAY);
		tmpbuf = sdns->sdns_y_bil_stren_array;
		break;
	case TISP_PARAM_SDNS_Y_BIL_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_BIL_NPV_ARRAY);
		tmpbuf = sdns->sdns_y_bil_npv_array;
		break;
	case TISP_PARAM_SDNS_C_BAS_WEI_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_C_BAS_WEI_ARRAY);
		tmpbuf = sdns->sdns_c_bas_wei_array;
		break;
	case TISP_PARAM_SDNS_C_FUS_MOD_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_C_FUS_MOD_ARRAY);
		tmpbuf = sdns->sdns_c_fus_mod_array;
		break;
	case TISP_PARAM_SDNS_C_FLU_CAL_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_C_FLU_CAL_ARRAY);
		tmpbuf = sdns->sdns_c_flu_cal_array;
		break;
	case TISP_PARAM_SDNS_C_FLU_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_C_FLU_STREN_ARRAY);
		tmpbuf = sdns->sdns_c_flu_stren_array;
		break;
	case TISP_PARAM_SDNS_C_FLU_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_C_FLU_NPV_ARRAY);
		tmpbuf = sdns->sdns_c_flu_npv_array;
		break;
	default:
		ISP_ERROR("%s,%d: sdns not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	memcpy(buf, tmpbuf, len);
	*plen = len;

	return 0;
}

int32_t tisp_sdns_param_array_set(tisp_sdns_t *sdns, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_SDNS_TOP_FUNC_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_TOP_FUNC_ARRAY);
		tmpbuf = sdns->sdns_top_func_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_DTL_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_THRES_ARRAY);
		tmpbuf = sdns->sdns_y_dtl_thres_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_FUS_SLOPE_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_FUS_SLOPE_ARRAY);
		tmpbuf = sdns->sdns_y_fus_slope_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_LUM_DIVOP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_LUM_DIVOP_ARRAY);
		tmpbuf = sdns->sdns_y_lum_divop_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_DTL_SEGOP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_SEGOP_ARRAY);
		tmpbuf = sdns->sdns_y_dtl_segop_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_FUS_SEGOP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_FUS_SEGOP_ARRAY);
		tmpbuf = sdns->sdns_y_fus_segop_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_LUM_SEGOP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_LUM_SEGOP_ARRAY);
		tmpbuf = sdns->sdns_y_lum_segop_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_DSP_SEGOP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DSP_SEGOP_ARRAY);
		tmpbuf = sdns->sdns_y_dsp_segop_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_BSP_SEGOP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_BSP_SEGOP_ARRAY);
		tmpbuf = sdns->sdns_y_bsp_segop_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_DTL_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_STREN_ARRAY);
		tmpbuf = sdns->sdns_y_dtl_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_FUS_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_FUS_STREN_ARRAY);
		tmpbuf = sdns->sdns_y_fus_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_LUM_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_LUM_STREN_ARRAY);
		tmpbuf = sdns->sdns_y_lum_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_DSP_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DSP_STREN_ARRAY);
		tmpbuf = sdns->sdns_y_dsp_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_BSP_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_BSP_STREN_ARRAY);
		tmpbuf = sdns->sdns_y_bsp_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_DTL_NPV_0_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_0_ARRAY);
		tmpbuf = sdns->sdns_y_dtl_npv_0_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_DTL_NPV_1_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_1_ARRAY);
		tmpbuf = sdns->sdns_y_dtl_npv_1_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_DTL_NPV_2_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_2_ARRAY);
		tmpbuf = sdns->sdns_y_dtl_npv_2_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_DTL_NPV_3_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_3_ARRAY);
		tmpbuf = sdns->sdns_y_dtl_npv_3_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_DTL_NPV_4_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DTL_NPV_4_ARRAY);
		tmpbuf = sdns->sdns_y_dtl_npv_4_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_FUS_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_FUS_NPV_ARRAY);
		tmpbuf = sdns->sdns_y_fus_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_LUM_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_LUM_NPV_ARRAY);
		tmpbuf = sdns->sdns_y_lum_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_DSP_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_DSP_NPV_ARRAY);
		tmpbuf = sdns->sdns_y_dsp_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_BSP_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_BSP_NPV_ARRAY);
		tmpbuf = sdns->sdns_y_bsp_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_BIL_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_BIL_STREN_ARRAY);
		tmpbuf = sdns->sdns_y_bil_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_Y_BIL_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_Y_BIL_NPV_ARRAY);
		tmpbuf = sdns->sdns_y_bil_npv_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_C_BAS_WEI_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_C_BAS_WEI_ARRAY);
		tmpbuf = sdns->sdns_c_bas_wei_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_C_FUS_MOD_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_C_FUS_MOD_ARRAY);
		tmpbuf = sdns->sdns_c_fus_mod_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_C_FLU_CAL_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_C_FLU_CAL_ARRAY);
		tmpbuf = sdns->sdns_c_flu_cal_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_C_FLU_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_C_FLU_STREN_ARRAY);
		tmpbuf = sdns->sdns_c_flu_stren_array;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_SDNS_C_FLU_NPV_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SDNS_C_FLU_NPV_ARRAY);
		tmpbuf = sdns->sdns_c_flu_npv_array;
		memcpy(tmpbuf, buf, len);
		tisp_sdns_all_reg_refresh(sdns, sdns->gain_old + 0x200);
		break;
	default:
		ISP_ERROR("%s,%d: sdns not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	return 0;
}
