#include <linux/kernel.h>
#include <linux/irqreturn.h>
#include <linux/dma-mapping.h>

#include "fix_point_calc.h"
#include "../inc/tiziano_sys.h"
#include "../inc/tiziano_core.h"
#include "tiziano_core.h"
#include "../inc/tiziano_isp.h"
#include "tiziano_lsc.h"
#include "tiziano_params.h"
#include "tiziano_map.h"

static uint32_t tiziano_lsc_ct_points[4] = {2900,3500,4700,5500};

int tiziano_lsc_lut_parameter(tisp_lsc_t *lsc)
{
	uint32_t value0 = 0;
	uint32_t value1 = 0;
	uint32_t value2 = 0;
	uint32_t wdata_l = 0;
	uint32_t wdata_h = 0;
	uint32_t i = 0;

	system_reg_write(((tisp_core_t *)lsc->core)->priv_data, LSC_ADDR_MESH_SCALE, lsc->mesh_scale);
	system_reg_write(((tisp_core_t *)lsc->core)->priv_data, LSC_ADDR_LUT_STRIDE, lsc->lut_stride);
	system_reg_write(((tisp_core_t *)lsc->core)->priv_data, LSC_ADDR_MESH_SIZE, ((lsc->mesh_size[1] << 16) | lsc->mesh_size[0]));

	for (i = 0; i < lsc->lut_num; i = i + 3) {
		value0 = lsc->d_linear[i];
		value1 = lsc->d_linear[i+1];
		value2 = lsc->d_linear[i+2];
		wdata_l = ((value1 & 0xff) << 16) | (value0 & 0xffff);
		wdata_h = ((value2 & 0xffff) << 8) | ((value1 & 0xff00) >> 8);

		system_reg_write(((tisp_core_t *)lsc->core)->priv_data, LSC_MEM_BASE + (i / 3) * 8, wdata_l);
		system_reg_write(((tisp_core_t *)lsc->core)->priv_data, LSC_MEM_BASE + (i / 3) * 8 + 4, wdata_h);
	}

	return 0;
}

uint32_t jz_isp_lsc_interpolate(tisp_lsc_t *lsc, uint32_t ct, uint32_t point_h,
				uint32_t point_l, uint32_t value_h,
				uint32_t value_l)
{
	/*printf("%d, %d, value = %d,ct:%d, point_h:%d,point_l:%d\n",(point_h - ct)/(point_h - point_l)*value_l, (ct - point_l)/(point_h - point_l)*value_h, value, ct, point_h, point_l);*/
	uint32_t value_h_h = value_h >> 8;
	uint32_t value_h_l = value_h & 0xff;
	uint32_t value_l_h = value_l >> 8;
	uint32_t value_l_l = value_l & 0xff;
	uint32_t value_out_l, value_out_h;

	value_out_l = (value_l_l * (point_h-point_l) +
		       (value_h_l-value_l_l) * (ct-point_l)) / (point_h-point_l);
	value_out_h = (value_l_h * (point_h-point_l) +
		       (value_h_h-value_l_h) * (ct-point_l)) / (point_h-point_l);

	return ((value_out_h << 8) | value_out_l);
}

int32_t jz_isp_lsc_ct(tisp_lsc_t *lsc)
{
	uint32_t i, j;
	uint32_t value[3];
	uint32_t wdata_l = 0;
	uint32_t wdata_h = 0;
	uint32_t wdata_l_0 = 0;
	uint32_t wdata_l_1 = 0;
	uint32_t wdata_l_2 = 0;
	uint32_t wdata_h_0 = 0;
	uint32_t wdata_h_1 = 0;
	uint32_t wdata_h_2 = 0;

	uint32_t lsc_lut_size = lsc->lut_num;
	uint32_t lsc_par_limit = 0;
	uint32_t tiziano_lsc_d_point = lsc->tiziano_lsc_ct_points[3];
	uint32_t tiziano_lsc_t_point_r = lsc->tiziano_lsc_ct_points[2];
	uint32_t tiziano_lsc_t_point_l = lsc->tiziano_lsc_ct_points[1];
	uint32_t tiziano_lsc_a_point = lsc->tiziano_lsc_ct_points[0];

	uint32_t lsc_str = 4096;
	int32_t x_int, x_fra;

	if (lsc->lsc_gain_old == 0xffffffff) {
		lsc_str = 4096;
	} else {
		x_int = lsc->lsc_gain_old >> 16;
		x_fra = lsc->lsc_gain_old & 0xffff;
		lsc_str = tisp_simple_intp(x_int, x_fra, lsc->mesh_lsc_str);
	}

	if (lsc->ct > tiziano_lsc_d_point)
		lsc->lsc_change_flag = 0;
	else if ((lsc->ct <= tiziano_lsc_d_point) && (lsc->ct > tiziano_lsc_t_point_r))
		lsc->lsc_change_flag = 1;
	else if ((lsc->ct <= tiziano_lsc_t_point_r) && (lsc->ct > tiziano_lsc_t_point_l))
		lsc->lsc_change_flag = 2;
	else if ((lsc->ct <= tiziano_lsc_t_point_l) && (lsc->ct > tiziano_lsc_a_point))
		lsc->lsc_change_flag = 3;
	else if (lsc->ct <= tiziano_lsc_a_point)
		lsc->lsc_change_flag = 4;

	if ((((lsc->lsc_change_flag == 0 && lsc->lsc_change_flag_last == 0)) ||
	     ((lsc->lsc_change_flag == 2 && lsc->lsc_change_flag_last == 2)) ||
	     ((lsc->lsc_change_flag == 4 && lsc->lsc_change_flag_last == 4)) ||
	     ((lsc->lsc_change_flag == 1 && (ISPSUBABS(lsc->ct, lsc->ct_last) < 300))) ||
	     ((lsc->lsc_change_flag == 3 && (ISPSUBABS(lsc->ct, lsc->ct_last) < 300)))) && lsc->gain_no_change){
		goto no_change;
	}
	for (i = 0; i < lsc_lut_size; i = i + 3) {
		switch (lsc->lsc_change_flag) {
		case 0:
			for(j = i; j < i + 3; j++){
				value[j-i] = lsc->d_linear[j];
			}
			break;
		case 1:
			for(j = i; j < i + 3; j++){
				value[j - i]=jz_isp_lsc_interpolate(lsc, lsc->ct, tiziano_lsc_d_point, tiziano_lsc_t_point_r, lsc->d_linear[j], lsc->t_linear[j]);
			}
			break;
		case 2:
			for(j = i; j < i + 3; j++){
				value[j - i] = lsc->t_linear[j];
			}
			break;
		case 3:
			for(j = i; j < i + 3; j++){
				value[j - i] = jz_isp_lsc_interpolate(lsc, lsc->ct, tiziano_lsc_t_point_l, tiziano_lsc_a_point, lsc->t_linear[j], lsc->a_linear[j]);
			}
			break;
		case 4:
			for(j=i;j<i+3;j++){
				value[j-i] = lsc->a_linear[j];
			}
			break;
		default:
			break;
		}
		wdata_l = ((value[1] & 0xff) << 16) | (value[0] & 0xffff);
		wdata_h = ((value[2] & 0xffff) << 8) | ((value[1] & 0xff00) >> 8);

		wdata_l_0 = (wdata_l) & 0xff;
		wdata_l_1 = (wdata_l >> 8) & 0xff;
		wdata_l_2 = (wdata_l >> 16) & 0xff;
		wdata_h_0 = (wdata_h) & 0xff;
		wdata_h_1 = (wdata_h >> 8) & 0xff;
		wdata_h_2 = (wdata_h >> 16) & 0xff;

		wdata_l_0 = wdata_l_0 * lsc_str / 4096;
		wdata_l_1 = wdata_l_1 * lsc_str / 4096;
		wdata_l_2 = wdata_l_2 * lsc_str / 4096;
		wdata_h_0 = wdata_h_0 * lsc_str / 4096;
		wdata_h_1 = wdata_h_1 * lsc_str / 4096;
		wdata_h_2 = wdata_h_2 * lsc_str / 4096;

		switch (lsc->mesh_scale) {
		case 0:
			lsc_par_limit = 128;
			break;
		case 1:
			lsc_par_limit = 64 ;
			break;
		case 2:
			lsc_par_limit = 32 ;
			break;
		case 3:
			lsc_par_limit = 16 ;
			break;
		default:
			ISP_ERROR("mesh scale is failed\n");
		}

		if (wdata_l_0 < lsc_par_limit)
			wdata_l_0 = lsc_par_limit;
		if (wdata_l_1 < lsc_par_limit)
			wdata_l_1 = lsc_par_limit;
		if (wdata_l_2 < lsc_par_limit)
			wdata_l_2 = lsc_par_limit;
		if (wdata_h_0 < lsc_par_limit)
			wdata_h_0 = lsc_par_limit;
		if (wdata_h_1 < lsc_par_limit)
			wdata_h_1 = lsc_par_limit;
		if (wdata_h_2 < lsc_par_limit)
			wdata_h_2 = lsc_par_limit;

		wdata_l = ((wdata_l_2 & 0xff) << 16) |
			((wdata_l_1 & 0xff) << 8) |
			(wdata_l_0 & 0xff) ;
		wdata_h = ((wdata_h_2 & 0xff) << 16) |
			((wdata_h_1 & 0xff) << 8) |
			(wdata_h_0 & 0xff) ;

		/* ISP_ERROR("%d,%d,",wdata_l,wdata_h); */
		system_reg_write(((tisp_core_t *)lsc->core)->priv_data, LSC_MEM_BASE + (i / 3) * 8, wdata_l);
		system_reg_write(((tisp_core_t *)lsc->core)->priv_data, LSC_MEM_BASE + (i / 3) * 8 + 4, wdata_h);
	}

no_change:
	lsc->lsc_change_flag_last = lsc->lsc_change_flag;

	return 0;
}

int32_t tisp_lsc_ct_update(tisp_lsc_t *lsc, uint64_t data1)
{
	lsc->ct = data1;
	/* ISP_ERROR("%s,%d: _ev = 0x%llx\n", __func__, __LINE__, _ev); */
	jz_isp_lsc_ct(lsc);
	lsc->ct_last = lsc->ct;

	return 0;
}

int32_t jz_isp_lsc_gain(tisp_lsc_t *lsc)
{
	return 0;
}

int32_t tisp_lsc_gain_update(tisp_lsc_t *lsc, uint64_t data1)
{
	uint32_t lsc_gain_diff;

	if (lsc->lsc_gain_old == 0xffffffff){
		lsc->lsc_gain_old = data1;
	} else {
		lsc_gain_diff = ISPSUBABS(data1,lsc->lsc_gain_old);
		if (lsc_gain_diff >= lsc->lsc_gain_thres) {
			lsc->lsc_gain_old = data1;
			lsc->gain_no_change = 0;
			jz_isp_lsc_ct(lsc);
			lsc->gain_no_change = 1;
		}
	}

	return 0;
}

int tiziano_lsc_params_refresh(tisp_lsc_t *lsc)
{
	memcpy(&lsc->lut_num, tparams.params_data.TISP_PARAM_LSC_LUT_NUM,
	       sizeof(tparams.params_data.TISP_PARAM_LSC_LUT_NUM));
	memcpy(&lsc->mesh_scale, tparams.params_data.TISP_PARAM_LSC_MESH_SCALE,
	       sizeof(tparams.params_data.TISP_PARAM_LSC_MESH_SCALE));
	memcpy(&lsc->lut_stride, tparams.params_data.TISP_PARAM_LSC_LUT_STRIDE,
	       sizeof(tparams.params_data.TISP_PARAM_LSC_LUT_STRIDE));
	memcpy(lsc->mesh_size, tparams.params_data.TISP_PARAM_LSC_MESH_SIZE,
	       sizeof(tparams.params_data.TISP_PARAM_LSC_MESH_SIZE));
	memcpy(lsc->tiziano_lsc_ct_points, tparams.params_data.TISP_PARAM_LSC_CT_POINTS,
	       sizeof(tparams.params_data.TISP_PARAM_LSC_CT_POINTS));
	memcpy(lsc->a_linear, tparams.params_data.TISP_PARAM_LSC_A_LINEAR,
	       sizeof(tparams.params_data.TISP_PARAM_LSC_A_LINEAR));
	memcpy(lsc->t_linear, tparams.params_data.TISP_PARAM_LSC_T_LINEAR,
	       sizeof(tparams.params_data.TISP_PARAM_LSC_T_LINEAR));
	memcpy(lsc->d_linear, tparams.params_data.TISP_PARAM_LSC_D_LINEAR,
	       sizeof(tparams.params_data.TISP_PARAM_LSC_D_LINEAR));
	memcpy(lsc->mesh_lsc_str, tparams.params_data.TISP_PARAM_LSC_MESH_STR,
	       sizeof(tparams.params_data.TISP_PARAM_LSC_MESH_STR));
	return 0;
}

int tiziano_lsc_dn_params_refresh(tisp_lsc_t *lsc)
{
	tiziano_lsc_params_refresh(lsc);

	return 0;
}

int tiziano_lsc_init(tisp_lsc_t *lsc)
{
	/*member init*/
	memcpy(lsc->tiziano_lsc_ct_points, tiziano_lsc_ct_points, sizeof(tiziano_lsc_ct_points));
	lsc->ct = 5000;
	lsc->ct_last = 5000;
	lsc->lsc_gain_thres = 0x100;
	lsc->lsc_gain_old = 0xffffffff;

	tiziano_lsc_params_refresh(lsc);
	tiziano_lsc_lut_parameter(lsc);

	return 0;
}

int32_t tisp_lsc_param_array_get(tisp_lsc_t *lsc, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_LSC_LUT_NUM:
		len = sizeof(tparams.params_data.TISP_PARAM_LSC_LUT_NUM);
		tmpbuf = &lsc->lut_num;
		break;
	case TISP_PARAM_LSC_MESH_SCALE:
		len = sizeof(tparams.params_data.TISP_PARAM_LSC_MESH_SCALE);
		tmpbuf = &lsc->mesh_scale;
		break;
	case TISP_PARAM_LSC_LUT_STRIDE:
		len = sizeof(tparams.params_data.TISP_PARAM_LSC_LUT_STRIDE);
		tmpbuf = &lsc->lut_stride;
		break;
	case TISP_PARAM_LSC_MESH_SIZE:
		len = sizeof(tparams.params_data.TISP_PARAM_LSC_MESH_SIZE);
		tmpbuf = lsc->mesh_size;
		break;
	case TISP_PARAM_LSC_CT_POINTS:
		len = sizeof(tparams.params_data.TISP_PARAM_LSC_CT_POINTS);
		tmpbuf = lsc->tiziano_lsc_ct_points;
		break;
	case TISP_PARAM_LSC_A_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_LSC_A_LINEAR);
		tmpbuf = lsc->a_linear;
		break;
	case TISP_PARAM_LSC_T_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_LSC_T_LINEAR);
		tmpbuf = lsc->t_linear;
		break;
	case TISP_PARAM_LSC_D_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_LSC_D_LINEAR);
		tmpbuf = lsc->d_linear;
		break;
	case TISP_PARAM_LSC_MESH_STR:
		len = sizeof(tparams.params_data.TISP_PARAM_LSC_MESH_STR);
		tmpbuf = lsc->mesh_lsc_str;
		break;
	default:
		ISP_ERROR("%s,%d: lsc not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	memcpy(buf, tmpbuf, len);
	*plen = len;

	return 0;
}

int32_t tisp_lsc_param_array_set(tisp_lsc_t *lsc, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_LSC_LUT_NUM:
		len = sizeof(tparams.params_data.TISP_PARAM_LSC_LUT_NUM);
		tmpbuf = &lsc->lut_num;
		break;
	case TISP_PARAM_LSC_MESH_SCALE:
		len = sizeof(tparams.params_data.TISP_PARAM_LSC_MESH_SCALE);
		tmpbuf = &lsc->mesh_scale;
		break;
	case TISP_PARAM_LSC_LUT_STRIDE:
		len = sizeof(tparams.params_data.TISP_PARAM_LSC_LUT_STRIDE);
		tmpbuf = &lsc->lut_stride;
		break;
	case TISP_PARAM_LSC_MESH_SIZE:
		len = sizeof(tparams.params_data.TISP_PARAM_LSC_MESH_SIZE);
		tmpbuf = lsc->mesh_size;
		break;
	case TISP_PARAM_LSC_CT_POINTS:
		len = sizeof(tparams.params_data.TISP_PARAM_LSC_CT_POINTS);
		tmpbuf = lsc->tiziano_lsc_ct_points;
		break;
	case TISP_PARAM_LSC_A_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_LSC_A_LINEAR);
		tmpbuf = lsc->a_linear;
		break;
	case TISP_PARAM_LSC_T_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_LSC_T_LINEAR);
		tmpbuf = lsc->t_linear;
		break;
	case TISP_PARAM_LSC_D_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_LSC_D_LINEAR);
		tmpbuf = lsc->d_linear;
		break;
	case TISP_PARAM_LSC_MESH_STR:
		len = sizeof(tparams.params_data.TISP_PARAM_LSC_MESH_STR);
		tmpbuf = lsc->mesh_lsc_str;
		break;
	default:
		ISP_ERROR("%s,%d: lsc not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}
	memcpy(tmpbuf, buf, len);

	system_reg_write(((tisp_core_t *)lsc->core)->priv_data, LSC_ADDR_MESH_SCALE, lsc->mesh_scale);
	system_reg_write(((tisp_core_t *)lsc->core)->priv_data, LSC_ADDR_LUT_STRIDE, lsc->lut_stride);
	system_reg_write(((tisp_core_t *)lsc->core)->priv_data, LSC_ADDR_MESH_SIZE, (lsc->mesh_size[1] << 16) | lsc->mesh_size[0]);
	lsc->lsc_change_flag_last = 5;
	lsc->ct_last = lsc->ct + 500;
	lsc->gain_no_change = 0;
	jz_isp_lsc_ct(lsc);
	lsc->gain_no_change = 1;

	return 0;
}
