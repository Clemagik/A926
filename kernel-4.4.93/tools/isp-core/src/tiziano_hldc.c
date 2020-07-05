#include <linux/kernel.h>
#include <linux/irqreturn.h>
#include <linux/dma-mapping.h>

#include "fix_point_calc.h"
#include "../inc/tiziano_sys.h"
#include "tiziano_core.h"
#include "tiziano_hldc.h"
#include "tiziano_params.h"
#include "tiziano_map.h"


/* sharpen parameters */
//uint32_t hldc_con_par_array[18];

/* hldc par config */
int32_t tisp_hldc_con_par_cfg(tisp_hldc_t *hldc)
{
	uint32_t reg_value;

	reg_value = (system_reg_read(((tisp_core_t *)hldc->core)->priv_data, HLDC_ADDR_REG_CTRL) & 0x3) |
		(hldc->hldc_con_par_array[0] << 8 ) |
		(hldc->hldc_con_par_array[1] << 12) |
		(hldc->hldc_con_par_array[2] << 16) |
		(hldc->hldc_con_par_array[3] << 20);
	system_reg_write(((tisp_core_t *)hldc->core)->priv_data, HLDC_ADDR_REG_CTRL, reg_value);
	reg_value = (hldc->hldc_con_par_array[4]) |
		(hldc->hldc_con_par_array[5] << 16);
	system_reg_write(((tisp_core_t *)hldc->core)->priv_data, HLDC_ADDR_DIS_Y_COEFF_X, reg_value);
	reg_value = (hldc->hldc_con_par_array[11]) |
		(hldc->hldc_con_par_array[12] << 16);
	system_reg_write(((tisp_core_t *)hldc->core)->priv_data, HLDC_ADDR_DIS_UV_COEFF_X, reg_value);
	system_reg_write(((tisp_core_t *)hldc->core)->priv_data, HLDC_ADDR_P1_Y_VAL, hldc->hldc_con_par_array[6]);
	system_reg_write(((tisp_core_t *)hldc->core)->priv_data, HLDC_ADDR_P1_UV_VAL, hldc->hldc_con_par_array[13]);
	system_reg_write(((tisp_core_t *)hldc->core)->priv_data, HLDC_ADDR_R2_Y_REP, hldc->hldc_con_par_array[7]);
	system_reg_write(((tisp_core_t *)hldc->core)->priv_data, HLDC_ADDR_R2_UV_REP, hldc->hldc_con_par_array[14]);
	reg_value = (hldc->hldc_con_par_array[8]) |
		(hldc->hldc_con_par_array[9] << 16);
	system_reg_write(((tisp_core_t *)hldc->core)->priv_data, HLDC_ADDR_Y_OFFSET, reg_value);
	reg_value = (hldc->hldc_con_par_array[15]) |
		(hldc->hldc_con_par_array[16] << 16);
	system_reg_write(((tisp_core_t *)hldc->core)->priv_data, HLDC_ADDR_UV_OFFSET, reg_value);
	system_reg_write(((tisp_core_t *)hldc->core)->priv_data, HLDC_ADDR_LINE_BLK_Y, hldc->hldc_con_par_array[10]);
	system_reg_write(((tisp_core_t *)hldc->core)->priv_data, HLDC_ADDR_LINE_BLK_UV, hldc->hldc_con_par_array[17]);

	return 0;
}

uint32_t tisp_hldc_par_refresh(tisp_hldc_t *hldc, uint32_t shadow_enable)
{
	uint32_t reg_value;
	/* shadow_enable: */
	/* 0: shadow disable */
	/* 1: shadow enable */

	tisp_hldc_con_par_cfg(hldc);
	if(shadow_enable == 1){
		reg_value = (system_reg_read(((tisp_core_t *)hldc->core)->priv_data, HLDC_ADDR_REG_CTRL) & 0xffffff00) | 0x11;
		system_reg_write(((tisp_core_t *)hldc->core)->priv_data, HLDC_ADDR_REG_CTRL, reg_value);
	}

	return 0;
}

int32_t tiziano_hldc_params_refresh(tisp_hldc_t *hldc)
{
	memcpy(hldc->hldc_con_par_array,
	       tparams.params_data.TISP_PARAM_HLDC_CON_PAR_ARRAY,
	       sizeof(tparams.params_data.TISP_PARAM_HLDC_CON_PAR_ARRAY));

	return 0;
}

int32_t tiziano_hldc_init(tisp_hldc_t *hldc)
{
	tiziano_hldc_params_refresh(hldc);
	tisp_hldc_par_refresh(hldc, 1);

	return 0;
}

int32_t tisp_hldc_param_array_get(tisp_hldc_t *hldc, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_HLDC_CON_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_HLDC_CON_PAR_ARRAY);
		tmpbuf = hldc->hldc_con_par_array;
		break;
	default:
		ISP_ERROR("%s,%d: hldc not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	memcpy(buf, tmpbuf, len);
	*plen = len;

	return 0;
}

int32_t tisp_hldc_param_array_set(tisp_hldc_t *hldc, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	int count = 0;
	int ret;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_HLDC_CON_PAR_ARRAY:
		len = sizeof(tparams.params_data.TISP_PARAM_HLDC_CON_PAR_ARRAY);
		tmpbuf = hldc->hldc_con_par_array;
		memcpy(tmpbuf, buf, len);
		system_reg_write(((tisp_core_t *)hldc->core)->priv_data, INPUT_CTRL_ADDR_CONTROL , 0x11);
		while(count < 60){
			ret = system_reg_read(((tisp_core_t *)hldc->core)->priv_data, MSCA_ADDR_DS0_DMA_OUT_DBG2_SEL);
			if(0x1000 == (ret&0x1000)){
				count =0;
				break;
			}else
				count ++;
		}
		tisp_hldc_par_refresh(hldc, 1);
		system_reg_write(((tisp_core_t *)hldc->core)->priv_data, INPUT_CTRL_ADDR_CONTROL , 0x10);
		system_reg_write(((tisp_core_t *)hldc->core)->priv_data, INPUT_CTRL_ADDR_IP_TRIG , 0x1);
		break;
	default:
		ISP_ERROR("%s,%d: hldc not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	return 0;
}
