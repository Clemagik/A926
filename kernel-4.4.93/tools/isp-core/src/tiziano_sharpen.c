#include <linux/kernel.h>
#include <linux/irqreturn.h>
#include <linux/dma-mapping.h>

#include "fix_point_calc.h"
#include "../inc/tiziano_sys.h"
#include "tiziano_core.h"
#include "../inc/tiziano_isp.h"
#include "tiziano_sharpen.h"
#include "tiziano_params.h"
#include "tiziano_map.h"

/* sharpen parameters */
#if 0
int32_t sharpen_uu_np_array[16];    //8 bits  0~255
int32_t sharpen_v1_sigma_np_array[16];    //5 bits  0~16
int32_t sharpen_w_wei_np_array[16];    //6 bits  0~32
int32_t sharpen_b_wei_np_array[16];    //6 bits  0~32
int32_t sharpen_uu_thres_array[9];
int32_t sharpen_uu_stren_array[9];
int32_t sharpen_uu_par_array[3];
int32_t sharpen_pixel_thres_array[9];
int32_t sharpen_w_stren_array[9];
int32_t sharpen_b_stren_array[9];
int32_t sharpen_brig_thres_array[9];
int32_t sharpen_dark_thres_array[9];
int32_t sharpen_con_par_array[8];

/* sharpen intp parameters */
uint32_t sharpen_uu_thres_intp;
uint32_t sharpen_uu_stren_intp;
uint32_t sharpen_pixel_thres_intp;
uint32_t sharpen_w_stren_intp;
uint32_t sharpen_b_stren_intp;
uint32_t sharpen_birg_thres_intp;
uint32_t sharpen_dark_thres_intp;
#endif

int32_t tiziano_sharpen_params_refresh(tisp_sharpen_t *sharpen)
{
	memcpy(sharpen->sharpen_uu_np_array, tparams.params_data.TISP_PARAM_SHARPEN_UU_NP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SHARPEN_UU_NP_ARRAY));
	memcpy(sharpen->sharpen_v1_sigma_np_array, tparams.params_data.TISP_PARAM_SHARPEN_V1_SIGMA_NP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SHARPEN_V1_SIGMA_NP_ARRAY));
	memcpy(sharpen->sharpen_w_wei_np_array, tparams.params_data.TISP_PARAM_SHARPEN_W_WEI_NP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SHARPEN_W_WEI_NP_ARRAY));
	memcpy(sharpen->sharpen_b_wei_np_array, tparams.params_data.TISP_PARAM_SHARPEN_B_WEI_NP_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SHARPEN_B_WEI_NP_ARRAY));
	memcpy(sharpen->sharpen_uu_thres_array, tparams.params_data.TISP_PARAM_SHARPEN_UU_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SHARPEN_UU_THRES_ARRAY));
	memcpy(sharpen->sharpen_uu_stren_array, tparams.params_data.TISP_PARAM_SHARPEN_UU_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SHARPEN_UU_STREN_ARRAY));
	memcpy(sharpen->sharpen_uu_par_array, tparams.params_data.TISP_PARAM_SHARPEN_UU_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SHARPEN_UU_PAR_ARRAY));
	memcpy(sharpen->sharpen_pixel_thres_array, tparams.params_data.TISP_PARAM_SHARPEN_PIXEL_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SHARPEN_PIXEL_THRES_ARRAY));
	memcpy(sharpen->sharpen_w_stren_array, tparams.params_data.TISP_PARAM_SHARPEN_W_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SHARPEN_W_STREN_ARRAY));
	memcpy(sharpen->sharpen_b_stren_array, tparams.params_data.TISP_PARAM_SHARPEN_B_STREN_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SHARPEN_B_STREN_ARRAY));
	memcpy(sharpen->sharpen_brig_thres_array, tparams.params_data.TISP_PARAM_SHARPEN_BRIG_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SHARPEN_BRIG_THRES_ARRAY));
	memcpy(sharpen->sharpen_dark_thres_array, tparams.params_data.TISP_PARAM_SHARPEN_DARK_THRES_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SHARPEN_DARK_THRES_ARRAY));
	memcpy(sharpen->sharpen_con_par_array, tparams.params_data.TISP_PARAM_SHARPEN_CON_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_SHARPEN_CON_PAR_ARRAY));

	return 0;
}

/* uu np value config */
int32_t tisp_sharpen_uu_np_cfg(tisp_sharpen_t *sharpen)
{
	uint32_t reg_value;

	reg_value = (sharpen->sharpen_uu_np_array[3] << 24) |
		(sharpen->sharpen_uu_np_array[2] << 16) |
		(sharpen->sharpen_uu_np_array[1] << 8) |
		(sharpen->sharpen_uu_np_array[0]);
	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_UU_NP_ARRAY_RG0, reg_value);

	reg_value = (sharpen->sharpen_uu_np_array[7] << 24) |
		(sharpen->sharpen_uu_np_array[6] << 16) |
		(sharpen->sharpen_uu_np_array[5] << 8) |
		(sharpen->sharpen_uu_np_array[4]);
	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_UU_NP_ARRAY_RG1, reg_value);

	reg_value = (sharpen->sharpen_uu_np_array[11] << 24) |
		(sharpen->sharpen_uu_np_array[10] << 16) |
		(sharpen->sharpen_uu_np_array[9 ] << 8) |
		(sharpen->sharpen_uu_np_array[8 ]);
	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_UU_NP_ARRAY_RG2, reg_value);

	reg_value = (sharpen->sharpen_uu_np_array[15] << 24) |
		(sharpen->sharpen_uu_np_array[14] << 16) |
		(sharpen->sharpen_uu_np_array[13] << 8) |
		(sharpen->sharpen_uu_np_array[12]);
	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_UU_NP_ARRAY_RG3, reg_value);

	return 0;
}

int32_t tisp_sharpen_v1_sigma_np_cfg(tisp_sharpen_t *sharpen)
{
	uint32_t reg_value;

	reg_value = (sharpen->sharpen_v1_sigma_np_array[5] << 25) |
		(sharpen->sharpen_v1_sigma_np_array[4] << 20) |
		(sharpen->sharpen_v1_sigma_np_array[3] << 15) |
		(sharpen->sharpen_v1_sigma_np_array[2] << 10) |
		(sharpen->sharpen_v1_sigma_np_array[1] << 5) |
		(sharpen->sharpen_v1_sigma_np_array[0]);

	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_V1_SIGMA_NP_ARRAY_RG0, reg_value);

	reg_value = (sharpen->sharpen_v1_sigma_np_array[11] << 25) |
		(sharpen->sharpen_v1_sigma_np_array[10] << 20) |
		(sharpen->sharpen_v1_sigma_np_array[9] << 15) |
		(sharpen->sharpen_v1_sigma_np_array[8] << 10) |
		(sharpen->sharpen_v1_sigma_np_array[7] << 5) |
		(sharpen->sharpen_v1_sigma_np_array[6]);
	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_V1_SIGMA_NP_ARRAY_RG1, reg_value);

	reg_value = (sharpen->sharpen_v1_sigma_np_array[15] << 25) |
		(sharpen->sharpen_v1_sigma_np_array[14] << 20) |
		(sharpen->sharpen_v1_sigma_np_array[13] << 15) |
		(sharpen->sharpen_v1_sigma_np_array[12] << 10);
	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_V1_SIGMA_NP_ARRAY_RG2, reg_value);

	return 0;
}

int32_t tisp_sharpen_w_wei_np_cfg(tisp_sharpen_t *sharpen)
{
	uint32_t reg_value;

	reg_value = (sharpen->sharpen_w_wei_np_array[4] << 24) |
		(sharpen->sharpen_w_wei_np_array[3] << 18) |
		(sharpen->sharpen_w_wei_np_array[2] << 12) |
		(sharpen->sharpen_w_wei_np_array[1] << 6) |
		(sharpen->sharpen_w_wei_np_array[0]);
	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_W_WEI_NP_ARRAY_RG0, reg_value);

	reg_value = (sharpen->sharpen_w_wei_np_array[9] << 24) |
		(sharpen->sharpen_w_wei_np_array[8] << 18) |
		(sharpen->sharpen_w_wei_np_array[7] << 12) |
		(sharpen->sharpen_w_wei_np_array[6] << 6) |
		(sharpen->sharpen_w_wei_np_array[5]);
	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_W_WEI_NP_ARRAY_RG1, reg_value);

	reg_value = (sharpen->sharpen_w_wei_np_array[14] << 24) |
		(sharpen->sharpen_w_wei_np_array[13] << 18) |
		(sharpen->sharpen_w_wei_np_array[12] << 12) |
		(sharpen->sharpen_w_wei_np_array[11] << 6) |
		(sharpen->sharpen_w_wei_np_array[10]);
	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_W_WEI_NP_ARRAY_RG2, reg_value);

	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_W_WEI_NP_ARRAY_RG3, sharpen->sharpen_w_wei_np_array[15]);

	return 0;
}

int32_t tisp_sharpen_b_wei_np_cfg(tisp_sharpen_t *sharpen)
{
	uint32_t reg_value;

	reg_value = (sharpen->sharpen_b_wei_np_array[4] << 24) |
		(sharpen->sharpen_b_wei_np_array[3] << 18) |
		(sharpen->sharpen_b_wei_np_array[2] << 12) |
		(sharpen->sharpen_b_wei_np_array[1] << 6) |
		(sharpen->sharpen_b_wei_np_array[0]);
	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_B_WEI_NP_ARRAY_RG0, reg_value);

	reg_value = (sharpen->sharpen_b_wei_np_array[9] << 24) |
		(sharpen->sharpen_b_wei_np_array[8] << 18) |
		(sharpen->sharpen_b_wei_np_array[7] << 12) |
		(sharpen->sharpen_b_wei_np_array[6] << 6) |
		(sharpen->sharpen_b_wei_np_array[5]);
	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_B_WEI_NP_ARRAY_RG1, reg_value);

	reg_value = (sharpen->sharpen_b_wei_np_array[14] << 24) |
		(sharpen->sharpen_b_wei_np_array[13] << 18) |
		(sharpen->sharpen_b_wei_np_array[12] << 12) |
		(sharpen->sharpen_b_wei_np_array[11] << 6) |
		(sharpen->sharpen_b_wei_np_array[10]);
	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_B_WEI_NP_ARRAY_RG2, reg_value);

	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_B_WEI_NP_ARRAY_RG3, sharpen->sharpen_b_wei_np_array[15]);

	return 0;
}

int32_t tisp_sharpen_uu_par_cfg(tisp_sharpen_t *sharpen)
{
	uint32_t reg_value;

	reg_value = (sharpen->sharpen_uu_par_array[0] << 18) |
		(sharpen->sharpen_uu_par_array[1] << 16) |
		(sharpen->sharpen_uu_thres_intp << 8) |
		(sharpen->sharpen_uu_stren_intp);
	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_STD_DIV_OPT, reg_value);

	reg_value = (system_reg_read(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_UU_SLOPE) & 0xfffff) |
		(sharpen->sharpen_uu_par_array[2] << 20);
	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_UU_SLOPE, reg_value);

	return 0;
}

int32_t tisp_sharpen_con_par_cfg(tisp_sharpen_t *sharpen)
{
	uint32_t data_tmp1;
	uint32_t reg_value;

	data_tmp1 = 8 - sharpen->sharpen_con_par_array[0];

	reg_value = (data_tmp1 << 20) |
		(sharpen->sharpen_con_par_array[0] << 16) |
		(sharpen->sharpen_con_par_array[1] << 8) |
		(sharpen->sharpen_pixel_thres_intp);
	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_V2_SIGMA, reg_value);

	reg_value = (sharpen->sharpen_con_par_array[2] << 26) |
		(sharpen->sharpen_w_stren_intp << 16) |
		(sharpen->sharpen_con_par_array[2] << 10) |
		(sharpen->sharpen_b_stren_intp);
	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_W_WEI_SEG_OPT, reg_value);

	reg_value = (sharpen->sharpen_con_par_array[6] << 16) |
		(sharpen->sharpen_con_par_array[7]);
	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_STREN, reg_value);

	reg_value = (sharpen->sharpen_birg_thres_intp << 16) |
		(sharpen->sharpen_dark_thres_intp);
	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_THRES, reg_value);

	reg_value = (system_reg_read(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_UU_SLOPE) & 0x3f00000) |
		(sharpen->sharpen_con_par_array[3] << 16) |
		(sharpen->sharpen_con_par_array[4] << 8) |
		(sharpen->sharpen_con_par_array[5]);
	system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_UU_SLOPE, reg_value);

	return 0;
}

int32_t tisp_sharpen_intp(tisp_sharpen_t *sharpen, int32_t gain_value)
{
	int32_t x_int, x_fra;

	x_int = gain_value >> 16;
	x_fra = gain_value & 0xffff;

	sharpen->sharpen_uu_thres_intp = tisp_simple_intp(x_int, x_fra, sharpen->sharpen_uu_thres_array);
	sharpen->sharpen_uu_stren_intp = tisp_simple_intp(x_int, x_fra, sharpen->sharpen_uu_stren_array);
	sharpen->sharpen_pixel_thres_intp = tisp_simple_intp(x_int, x_fra, sharpen->sharpen_pixel_thres_array);
	sharpen->sharpen_w_stren_intp = tisp_simple_intp(x_int, x_fra, sharpen->sharpen_w_stren_array);
	sharpen->sharpen_b_stren_intp = tisp_simple_intp(x_int, x_fra, sharpen->sharpen_b_stren_array);
	sharpen->sharpen_birg_thres_intp = tisp_simple_intp(x_int, x_fra, sharpen->sharpen_brig_thres_array);
	sharpen->sharpen_dark_thres_intp = tisp_simple_intp(x_int, x_fra, sharpen->sharpen_dark_thres_array);

	return 0;
}

int32_t tisp_sharpen_all_reg_refresh(tisp_sharpen_t *sharpen, int gain_value)
{
	tisp_sharpen_intp(sharpen, gain_value);
	tisp_sharpen_uu_np_cfg(sharpen);
	tisp_sharpen_v1_sigma_np_cfg(sharpen);
	tisp_sharpen_w_wei_np_cfg(sharpen);
	tisp_sharpen_b_wei_np_cfg(sharpen);
	tisp_sharpen_uu_par_cfg(sharpen);
	tisp_sharpen_con_par_cfg(sharpen);

	return 0;
}

int32_t tisp_sharpen_intp_reg_refresh(tisp_sharpen_t *sharpen, int gain_value)
{
	tisp_sharpen_intp(sharpen, gain_value);
	tisp_sharpen_uu_par_cfg(sharpen);
	tisp_sharpen_con_par_cfg(sharpen);

	return 0;
}

uint32_t tisp_sharpen_par_refresh(tisp_sharpen_t *sharpen, uint32_t gain_value, uint32_t gain_thres, uint32_t shadow_en)
{
	/* shadow_en: */
	/* 0: shadow disable */
	/* 1: shadow enable */

	uint32_t gain_diff;

	if (sharpen->gain_old == 0xffffffff) {
		sharpen->gain_old = gain_value;
		tisp_sharpen_all_reg_refresh(sharpen, gain_value);
	} else {
		if (gain_value >= sharpen->gain_old)
			gain_diff = gain_value-sharpen->gain_old;
		else
			gain_diff = sharpen->gain_old-gain_value;

		if (gain_diff >= gain_thres) {
			sharpen->gain_old = gain_value;
			tisp_sharpen_intp_reg_refresh(sharpen, gain_value);
		}
	}

	if (shadow_en == 1) {
		system_reg_write(((tisp_core_t *)sharpen->core)->priv_data, SHARPEN_ADDR_SYNC, 1);
	}

	return 0;
}

int32_t tiziano_sharpen_init(tisp_sharpen_t *sharpen)
{
	sharpen->gain_old = 0xffffffff;
	sharpen->gain_thres = 0x100;
	tiziano_sharpen_params_refresh(sharpen);
	tisp_sharpen_par_refresh(sharpen, 0x10000, 0x10000, 0);

	return 0;
}

int32_t tisp_sharpen_refresh(tisp_sharpen_t *sharpen, uint32_t gain_value)
{
	tisp_sharpen_par_refresh(sharpen, gain_value, sharpen->gain_thres, sharpen->shadow_en);

	return 0;
}

int32_t tiziano_sharpen_dn_params_refresh(tisp_sharpen_t *sharpen)
{
	tiziano_sharpen_params_refresh(sharpen);
	tisp_sharpen_all_reg_refresh(sharpen, sharpen->gain_old);

	return 0;
}

int32_t tisp_sharpen_param_array_get(tisp_sharpen_t *sharpen, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_SHARPEN_UU_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_UU_NP_ARRAY);
		tmpbuf = sharpen->sharpen_uu_np_array;
		break;
	case TISP_PARAM_SHARPEN_V1_SIGMA_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_V1_SIGMA_NP_ARRAY);
		tmpbuf = sharpen->sharpen_v1_sigma_np_array;
		break;
	case TISP_PARAM_SHARPEN_W_WEI_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_W_WEI_NP_ARRAY);
		tmpbuf = sharpen->sharpen_w_wei_np_array;
		break;
	case TISP_PARAM_SHARPEN_B_WEI_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_B_WEI_NP_ARRAY);
		tmpbuf = sharpen->sharpen_b_wei_np_array;
		break;
	case TISP_PARAM_SHARPEN_UU_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_UU_THRES_ARRAY);
		tmpbuf = sharpen->sharpen_uu_thres_array;
		break;
	case TISP_PARAM_SHARPEN_UU_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_UU_STREN_ARRAY);
		tmpbuf = sharpen->sharpen_uu_stren_array;
		break;
	case TISP_PARAM_SHARPEN_UU_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_UU_PAR_ARRAY);
		tmpbuf = sharpen->sharpen_uu_par_array;
		break;
	case TISP_PARAM_SHARPEN_PIXEL_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_PIXEL_THRES_ARRAY);
		tmpbuf = sharpen->sharpen_pixel_thres_array;
		break;
	case TISP_PARAM_SHARPEN_W_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_W_STREN_ARRAY);
		tmpbuf = sharpen->sharpen_w_stren_array;
		break;
	case TISP_PARAM_SHARPEN_B_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_B_STREN_ARRAY);
		tmpbuf = sharpen->sharpen_b_stren_array;
		break;
	case TISP_PARAM_SHARPEN_BRIG_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_BRIG_THRES_ARRAY);
		tmpbuf = sharpen->sharpen_brig_thres_array;
		break;
	case TISP_PARAM_SHARPEN_DARK_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_DARK_THRES_ARRAY);
		tmpbuf = sharpen->sharpen_dark_thres_array;
		break;
	case TISP_PARAM_SHARPEN_CON_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_CON_PAR_ARRAY);
		tmpbuf = sharpen->sharpen_con_par_array;
		break;
	default:
		ISP_ERROR("%s,%d: sharpen not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	memcpy(buf, tmpbuf, len);
	*plen = len;

	return 0;
}

int32_t tisp_sharpen_param_array_set(tisp_sharpen_t *sharpen, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_SHARPEN_UU_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_UU_NP_ARRAY);
		tmpbuf = sharpen->sharpen_uu_np_array;
		break;
	case TISP_PARAM_SHARPEN_V1_SIGMA_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_V1_SIGMA_NP_ARRAY);
		tmpbuf = sharpen->sharpen_v1_sigma_np_array;
		break;
	case TISP_PARAM_SHARPEN_W_WEI_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_W_WEI_NP_ARRAY);
		tmpbuf = sharpen->sharpen_w_wei_np_array;
		break;
	case TISP_PARAM_SHARPEN_B_WEI_NP_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_B_WEI_NP_ARRAY);
		tmpbuf = sharpen->sharpen_b_wei_np_array;
		break;
	case TISP_PARAM_SHARPEN_UU_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_UU_THRES_ARRAY);
		tmpbuf = sharpen->sharpen_uu_thres_array;
		break;
	case TISP_PARAM_SHARPEN_UU_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_UU_STREN_ARRAY);
		tmpbuf = sharpen->sharpen_uu_stren_array;
		break;
	case TISP_PARAM_SHARPEN_UU_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_UU_PAR_ARRAY);
		tmpbuf = sharpen->sharpen_uu_par_array;
		break;
	case TISP_PARAM_SHARPEN_PIXEL_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_PIXEL_THRES_ARRAY);
		tmpbuf = sharpen->sharpen_pixel_thres_array;
		break;
	case TISP_PARAM_SHARPEN_W_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_W_STREN_ARRAY);
		tmpbuf = sharpen->sharpen_w_stren_array;
		break;
	case TISP_PARAM_SHARPEN_B_STREN_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_B_STREN_ARRAY);
		tmpbuf = sharpen->sharpen_b_stren_array;
		break;
	case TISP_PARAM_SHARPEN_BRIG_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_BRIG_THRES_ARRAY);
		tmpbuf = sharpen->sharpen_brig_thres_array;
		break;
	case TISP_PARAM_SHARPEN_DARK_THRES_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_DARK_THRES_ARRAY);
		tmpbuf = sharpen->sharpen_dark_thres_array;
		break;
	case TISP_PARAM_SHARPEN_CON_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_SHARPEN_CON_PAR_ARRAY);
		tmpbuf = sharpen->sharpen_con_par_array;
		tisp_sharpen_all_reg_refresh(sharpen, sharpen->gain_old + 0x200);
		break;
	default:
		ISP_ERROR("%s,%d: sharpen not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	memcpy(tmpbuf, buf, len);

	return 0;
}
