#include <linux/kernel.h>
#include <linux/irqreturn.h>
#include <linux/dma-mapping.h>

#include "../inc/tiziano_core.h"

#include "fix_point_calc.h"
#include "tiziano_map.h"
#include "../inc/tiziano_sys.h"
#include "tiziano_gamma.h"
#include "tiziano_core.h"
#include "tiziano_params.h"

u32 tiziano_gamma_lut[129];

int tiziano_gamma_lut_parameter(tisp_gamma_t *gamma)
{
	int i;
	for (i = 0; i < TISP_PARAM_GAMMA_RGB_LUT_SADDR_COLS - 1; i++) {
		system_reg_write(((tisp_core_t *)gamma->core)->priv_data, R_GAMMA_MEM_BASE + i * 4,
				 (gamma->tiziano_gamma_lut[i + 1] << 12) |
				 gamma->tiziano_gamma_lut[i]);
		system_reg_write(((tisp_core_t *)gamma->core)->priv_data, G_GAMMA_MEM_BASE + i * 4,
				 (gamma->tiziano_gamma_lut[i + 1] << 12) |
				 gamma->tiziano_gamma_lut[i]);
		system_reg_write(((tisp_core_t *)gamma->core)->priv_data, B_GAMMA_MEM_BASE + i * 4,
				 (gamma->tiziano_gamma_lut[i + 1] << 12) |
				 gamma->tiziano_gamma_lut[i]);
	}

	return 0;
}

int tiziano_gamma_params_refresh(tisp_gamma_t *gamma)
{
	memcpy(gamma->tiziano_gamma_lut,
	       tparams.params_data.TISP_PARAM_GAMMA_RGB_LUT_SADDR,
	       sizeof(tparams.params_data.TISP_PARAM_GAMMA_RGB_LUT_SADDR));

	return 0;
}

int tiziano_gamma_dn_params_refresh(tisp_gamma_t *gamma)
{
	tiziano_gamma_params_refresh(gamma);
	tiziano_gamma_lut_parameter(gamma);

	return 0;
}

int tiziano_gamma_init(tisp_gamma_t *gamma)
{
	tiziano_gamma_params_refresh(gamma);
	tiziano_gamma_lut_parameter(gamma);

	return 0;
}

int32_t tisp_gamma_param_array_get(tisp_gamma_t *gamma, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_GAMMA_RGB_LUT_SADDR:
		len = sizeof(tparams.params_data.TISP_PARAM_GAMMA_RGB_LUT_SADDR);
		tmpbuf = gamma->tiziano_gamma_lut;
		break;
	default:
		ISP_ERROR("%s,%d: gamma not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}
	memcpy(buf, tmpbuf, len);
	*plen = len;

	return 0;
}

int32_t tisp_gamma_param_array_set(tisp_gamma_t *gamma, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_GAMMA_RGB_LUT_SADDR:
		len = sizeof(tparams.params_data.TISP_PARAM_GAMMA_RGB_LUT_SADDR);
		tmpbuf = gamma->tiziano_gamma_lut;
		memcpy(tmpbuf, buf, len);
		tiziano_gamma_lut_parameter(gamma);
		break;
	default:
		ISP_ERROR("%s,%d: gamma not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	return 0;
}
