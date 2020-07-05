#include <linux/kernel.h>
#include <linux/irqreturn.h>
#include <linux/dma-mapping.h>

#include "fix_point_calc.h"
#include "../inc/tiziano_sys.h"
#include "tiziano_core.h"
#include "tiziano_clm.h"
#include "../inc/tiziano_isp.h"
#include "tiziano_params.h"
#include "tiziano_map.h"

void clm_lut2reg(int *clm_s_lut, int *clm_h_lut, int *clm_s_reg, int *clm_h_reg)
{
	int i, j;

	for (i = 0; i < 30; i++){
		for (j = 0; j < 7; j++) {
			clm_s_reg[i * 7 * 2 + j * 2] = ((clm_s_lut[i * 7 * 5 + j * 5 + 3] & 0x1f) << 27) |
				((clm_s_lut[i * 7 * 5 + j * 5 + 2] & 0x1ff) << 18) |
				((clm_s_lut[i * 7 * 5 + j * 5 + 1] & 0x1ff) << 9) |
				((clm_s_lut[i * 7 * 5 + j * 5 + 0] & 0x1ff) << 0);
			clm_s_reg[i * 7 * 2 + j * 2 + 1] = ((clm_s_lut[i * 7 * 5 + j * 5 + 4] & 0x1ff) << 4) |
				((clm_s_lut[i * 7 * 5 + j * 5 + 3] & 0x1e0) >> 5);
			clm_h_reg[i * 7 * 2 + j * 2] = ((clm_h_lut[i * 7 * 5 + j * 5 + 4] & 0xf) << 28) |
				((clm_h_lut[i * 7 * 5 + j * 5 + 3] & 0x7f) << 21) |
				((clm_h_lut[i * 7 * 5 + j * 5 + 2] & 0x7f) << 14) |
				((clm_h_lut[i * 7 * 5 + j * 5 + 1] & 0x7f) << 7) |
				((clm_h_lut[i * 7 * 5 + j * 5 + 0] & 0x7f) << 0);
			clm_h_reg[i*7*2+j*2+1] = ((clm_h_lut[i*7*5+j*5+4] & 0x70) >> 4);
			//printf("trans %d %d\n",i,j);
		}
	}
}

int32_t tiziano_set_parameter_clm(tisp_clm_t *clm)
{
	int32_t i;

	clm_lut2reg(clm->tiziano_clm_s_lut, clm->tiziano_clm_h_lut, clm->tiziano_clm_s_reg,clm->tiziano_clm_h_reg);
	system_reg_write(((tisp_core_t *)clm->core)->priv_data, CLM_ADDR_LUT_SHIFT, clm->tiziano_clm_lut_shift);
	for (i = 0; i < 420; i++) {
		system_reg_write(((tisp_core_t *)clm->core)->priv_data, CLM_H_0_MEM_BASE + i * 4, clm->tiziano_clm_h_reg[i]);
	}
	for (i = 0; i < 420; i++) {
		system_reg_write(((tisp_core_t *)clm->core)->priv_data, CLM_H_1_MEM_BASE + i * 4, clm->tiziano_clm_h_reg[i]);
	}
	for (i = 0; i < 420; i++) {
		system_reg_write(((tisp_core_t *)clm->core)->priv_data, CLM_S_0_MEM_BASE + i * 4, clm->tiziano_clm_s_reg[i]);
	}
	for (i = 0; i < 420; i++) {
		system_reg_write(((tisp_core_t *)clm->core)->priv_data, CLM_S_1_MEM_BASE + i * 4, clm->tiziano_clm_s_reg[i]);
	}

	return 0;
}

int tiziano_clm_params_refresh(tisp_clm_t *clm)
{
	memcpy(clm->tiziano_clm_h_lut,
	       tparams.params_data.TISP_PARAM_CLM_H_LUT,
	       sizeof(tparams.params_data.TISP_PARAM_CLM_H_LUT));
	memcpy(clm->tiziano_clm_s_lut,
	       tparams.params_data.TISP_PARAM_CLM_S_LUT,
	       sizeof(tparams.params_data.TISP_PARAM_CLM_S_LUT));
	memcpy(&clm->tiziano_clm_lut_shift,
	       tparams.params_data.TISP_PARAM_CLM_LUT_SHIFT,
	       sizeof(tparams.params_data.TISP_PARAM_CLM_LUT_SHIFT));

	return 0;
}

int tiziano_clm_dn_params_refresh(tisp_clm_t *clm)
{
	tiziano_clm_params_refresh(clm);
	tiziano_set_parameter_clm(clm);

	return 0;
}

int tiziano_clm_init(tisp_clm_t *clm)
{
	tiziano_clm_params_refresh(clm);
	tiziano_set_parameter_clm(clm);

	return 0;
}

int32_t tisp_clm_param_array_get(tisp_clm_t *clm, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_CLM_H_LUT:
		len = sizeof(tparams.params_data.TISP_PARAM_CLM_H_LUT);
		tmpbuf = clm->tiziano_clm_h_lut;
		break;
	case TISP_PARAM_CLM_S_LUT:
		len = sizeof(tparams.params_data.TISP_PARAM_CLM_S_LUT);
		tmpbuf = clm->tiziano_clm_s_lut;
		break;
	case TISP_PARAM_CLM_LUT_SHIFT:
		len = sizeof(tparams.params_data.TISP_PARAM_CLM_LUT_SHIFT);
		tmpbuf = &clm->tiziano_clm_lut_shift;
		break;
	default:
		ISP_ERROR("%s,%d: clm not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	memcpy(buf, tmpbuf, len);
	*plen = len;

	return 0;
}

int32_t tisp_clm_param_array_set(tisp_clm_t *clm, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_CLM_H_LUT:
		len = sizeof(tparams.params_data.TISP_PARAM_CLM_H_LUT);
		tmpbuf = clm->tiziano_clm_h_lut;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_CLM_S_LUT:
		len = sizeof(tparams.params_data.TISP_PARAM_CLM_S_LUT);
		tmpbuf = clm->tiziano_clm_s_lut;
		memcpy(tmpbuf, buf, len);
		break;
	case TISP_PARAM_CLM_LUT_SHIFT:
		len = sizeof(tparams.params_data.TISP_PARAM_CLM_LUT_SHIFT);
		tmpbuf = &clm->tiziano_clm_lut_shift;
		memcpy(tmpbuf, buf, len);
		tiziano_set_parameter_clm(clm);
		break;
	default:
		ISP_ERROR("%s,%d: clm not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	return 0;
}
