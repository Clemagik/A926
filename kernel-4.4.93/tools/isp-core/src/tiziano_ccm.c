#include <linux/kernel.h>
#include <linux/irqreturn.h>
#include <linux/dma-mapping.h>

#include "fix_point_calc.h"
#include "../inc/tiziano_sys.h"
#include "tiziano_core.h"
#include "../inc/tiziano_isp.h"
#include "tiziano_ccm.h"
#include "tiziano_params.h"
#include "tiziano_map.h"

void cm_control(int *cm, uint32_t sat, int *cm_out);
/********************* CCM ALGORITHM ************************/
uint32_t tiziano_ccm_lut_parameter(tisp_ccm_t *ccm, uint32_t *_ccm_reg)
{
	uint32_t i;

	for (i = 0; i < TISP_PARAM_CCM_D_LINEAR_COLS; i += 2 ){
		if(i == (TISP_PARAM_CCM_D_LINEAR_COLS - 1)){
			ccm->tiziano_linear_value = *(_ccm_reg+i);
		} else {
			ccm->tiziano_linear_value= ((*(_ccm_reg + i + 1))<<16) | (*(_ccm_reg+i));
		}
		system_reg_write(((tisp_core_t *)ccm->core)->priv_data, CCM_ADDR_CCM_RRRG+i*2, ccm->tiziano_linear_value);
	}

	return 0;
}

void jz_isp_ccm_reg2par(int32_t *para, uint32_t *reg)
{
	int i;

	for (i = 0; i < 9; i++) {
		if (*(reg + i) > 8191) {
			*(para + i) = (16384 - *(reg + i)) * (-1);
		} else {
			*(para + i) = (*(reg + i));
		}
		//ISP_ERROR("%d ", *(para + i));
	}
	//ISP_ERROR("\n");
}

void jz_isp_ccm_para2reg(uint32_t *reg, int32_t *para)
{
	int i;

	for (i = 0; i < 9; i++) {
		if(*(para + i) < 0){
			*(reg + i) = (uint32_t)((~(short)(*(para + i) * (-1))) + 1) & 0x00003fff;
		} else {
			*(reg + i) = (uint32_t)(*(para + i));
		}
	}
}

void jz_isp_ccm_parameter_convert(tisp_ccm_t *ccm)
{
	jz_isp_ccm_reg2par(ccm->_ccm_a_parameter, ccm->tiziano_ccm_a_linear);
	jz_isp_ccm_reg2par(ccm->_ccm_t_parameter, ccm->tiziano_ccm_t_linear);
	jz_isp_ccm_reg2par(ccm->_ccm_d_parameter, ccm->tiziano_ccm_d_linear);
}


void tiziano_ct_ccm_interpolation(tisp_ccm_t *ccm, uint32_t cm_ct, uint32_t ccm_ct_tol)
{
	static uint32_t ct_list[4];
	static uint32_t ct_flag = 0;
	static uint32_t ct_flag_last = 0;
	int i=0;

	ct_list[0] = 2800 + ccm_ct_tol;
	ct_list[1] = 3800 - ccm_ct_tol;
	ct_list[2] = 3800 + ccm_ct_tol;
	ct_list[3] = 5000 - ccm_ct_tol;

	if (cm_ct > ct_list[3])
		ct_flag = 0;                                      // D
	else if ((cm_ct <= ct_list[3]) && (cm_ct > ct_list[2]))
		ct_flag = 1;                                      //interpolation
	else if ((cm_ct <= ct_list[2]) && (cm_ct > ct_list[1]))
		ct_flag = 2;                                      //T
	else if ((cm_ct <= ct_list[1]) && (cm_ct > ct_list[0]))
		ct_flag = 3;                                      //interpolation
	else if (cm_ct <= ct_list[0])
		ct_flag = 4;                                      //A

	if(ccm->ccm_real.first_tune == 0){
		if ((ct_flag == 0 && ct_flag_last == 0) ||
			(ct_flag == 2 && ct_flag_last == 2) ||
			(ct_flag == 4 && ct_flag_last == 4)) {
			return;
		}
	}
	ct_flag_last = ct_flag;

	switch (ct_flag) {
	case 0:
		memcpy(ccm->ccm_parameter, ccm->_ccm_d_parameter, sizeof(ccm->_ccm_d_parameter));
		break;
	case 1:
		for (i = 0; i < 9; i++) {
			ccm->ccm_parameter[i] = ISPINT(cm_ct, ct_list[2], ct_list[3],
						  ccm->_ccm_t_parameter[i], ccm->_ccm_d_parameter[i]);
		}
		break;
	case 2:
		memcpy(ccm->ccm_parameter, ccm->_ccm_t_parameter, sizeof(ccm->_ccm_t_parameter));
		break;
	case 3:
		for (i = 0; i < 9; i++) {
			ccm->ccm_parameter[i] = ISPINT(cm_ct, ct_list[0], ct_list[1],
						  ccm->_ccm_a_parameter[i], ccm->_ccm_t_parameter[i]);
		}
		break;
	case 4:
		memcpy(ccm->ccm_parameter, ccm->_ccm_a_parameter, sizeof(ccm->_ccm_a_parameter));
		break;
	default:
		break;
	}
}

int32_t jz_isp_ccm(tisp_ccm_t *ccm)
{
	uint32_t AePointPos = 10; //_AePointPos[0];
	uint32_t cm_ev = ccm->_ev >> AePointPos;
/* fix_point_mult3(AePointPos, _ae_reg[1], (uint32_t)_ae_reg[2]<<AePointPos, _ae_reg[3]); */
	uint32_t cm_ct = ccm->_ct;
	int32_t  _cm_out[9];
	uint32_t _cm_out_reg[9];
	uint32_t i = 0;
	uint32_t mode = 1;

	ccm->ccm_real.ct_threshold = 100;
#if 0 //awb list func usage
	uint32_t mode = cm_awb_list[0];
	ccm->ccm_real.ct_threshold = cm_awb_list[1];
#endif
	/**************** Ev update **************************/
	if ((ccm->ccm_real.first_tune == 1) || (ISPSUBABS(cm_ev,ccm->ccm_real.ev_old) > ccm->ccm_real.ev_threshold)) {
		for (i = 0; i < 9; i++) {
			if (cm_ev <= ccm->cm_ev_list[i]) {
				if (i == 0) {
					ccm->ccm_real.cm_sat = ccm->cm_sat_list[0];
					break;
				}
				if (ccm->cm_ev_list[i] == ccm->cm_ev_list[i-1]) {
					ccm->ccm_real.cm_sat = ccm->cm_sat_list[i];
					break;
				} else {
					ccm->ccm_real.cm_sat = ISPINT(cm_ev,ccm->cm_ev_list[i-1],ccm->cm_ev_list[i],ccm->cm_sat_list[i-1],ccm->cm_sat_list[i]);
					break;
				}
			}
		}
		if (i == 9) {
			ccm->ccm_real.cm_sat = ccm->cm_sat_list[8];
		}
		/* ISP_ERROR("%s:%d: cm_sat is %d\n",__func__,__LINE__,cm_sat); */
	}
	/**************** Ct update **************************/
	jz_isp_ccm_parameter_convert(ccm);
	if ((ccm->ccm_real.first_tune == 1) || (ISPSUBABS(cm_ct, ccm->ccm_real.ct_old) > ccm->ccm_real.ct_threshold)){
		if (mode == 0){
			if (cm_ct > 4500){
				memcpy(ccm->ccm_parameter, ccm->_ccm_d_parameter, sizeof(ccm->_ccm_d_parameter));
			} else if (cm_ct > 3100) {
				memcpy(ccm->ccm_parameter, ccm->_ccm_t_parameter, sizeof(ccm->_ccm_t_parameter));
			} else {
				memcpy(ccm->ccm_parameter, ccm->_ccm_a_parameter, sizeof(ccm->_ccm_a_parameter));
			}
		} else {
			tiziano_ct_ccm_interpolation(ccm, cm_ct,ccm->ccm_real.ct_threshold);
		}
	}

	cm_control(ccm->ccm_parameter, ccm->ccm_real.cm_sat, _cm_out);
	jz_isp_ccm_para2reg(_cm_out_reg, _cm_out);
	tiziano_ccm_lut_parameter(ccm, _cm_out_reg);

	ccm->ccm_real.first_tune = 0;

	return 0;
}

int32_t tisp_ccm_ev_update(tisp_ccm_t *ccm, uint64_t data1)
{
	uint32_t cm_ev = 0;

	ccm->_ev = data1;
	cm_ev = ccm->_ev >> 10;  //fix_point_mult3(AePointPos, _ae_reg[1],

	if (ISPSUBABS(cm_ev,ccm->ccm_real.ev_old) > ccm->ccm_real.ev_threshold) {
		jz_isp_ccm(ccm);
		ccm->ccm_real.ev_old = cm_ev;
	}

	return 0;
}

int32_t tisp_ccm_ct_update(tisp_ccm_t *ccm, uint64_t data1)
{
	uint32_t cm_ct = 0;

	ccm->_ct = data1;
	cm_ct = ccm->_ct;

	if (ISPSUBABS(cm_ct, ccm->ccm_real.ct_old) > ccm->ccm_real.ct_threshold) {
		jz_isp_ccm(ccm);
		ccm->ccm_real.ct_old = cm_ct;
	}

	return 0;
}


uint32_t tiziano_ccm_params_refresh(tisp_ccm_t *ccm)
{
	memcpy(ccm->tiziano_ccm_dp_cfg, tparams.params_data.TISP_PARAM_CCM_DP_CFG,
	       sizeof(tparams.params_data.TISP_PARAM_CCM_DP_CFG));
	memcpy(ccm->tiziano_ccm_a_linear, tparams.params_data.TISP_PARAM_CCM_A_LINEAR,
	       sizeof(tparams.params_data.TISP_PARAM_CCM_A_LINEAR));
	memcpy(ccm->tiziano_ccm_t_linear, tparams.params_data.TISP_PARAM_CCM_T_LINEAR,
	       sizeof(tparams.params_data.TISP_PARAM_CCM_T_LINEAR));
	memcpy(ccm->tiziano_ccm_d_linear,tparams.params_data.TISP_PARAM_CCM_D_LINEAR,
	       sizeof(tparams.params_data.TISP_PARAM_CCM_D_LINEAR));
	memcpy(ccm->cm_ev_list, tparams.params_data.TISP_PARAM_CCM_EV_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_CCM_EV_LIST));
	memcpy(ccm->cm_sat_list, tparams.params_data.TISP_PARAM_CCM_SAT_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_CCM_SAT_LIST));
	memcpy(ccm->cm_awb_list, tparams.params_data.TISP_PARAM_CCM_AWB_LIST,
	       sizeof(tparams.params_data.TISP_PARAM_CCM_AWB_LIST));

	return 0;
}

uint32_t tiziano_ccm_dn_params_refresh(tisp_ccm_t *ccm)
{
	tiziano_ccm_params_refresh(ccm);
	ccm->ccm_real.first_tune = 1;
	jz_isp_ccm(ccm);

	return 0;
}

uint32_t tiziano_ccm_init(tisp_ccm_t *ccm)
{

	ccm->_ev = (uint32_t)400<<10;
	ccm->_ct = 5000;

	ccm->ccm_real.ev_old = ccm->_ev >> 10;
	ccm->ccm_real.ct_old = ccm->_ct;
	ccm->ccm_real.cm_sat = 256;
	ccm->ccm_real.first_tune = 1;
	ccm->ccm_real.ct_threshold = 100;
	ccm->ccm_real.ev_threshold = 40;
#if 0 //awb list usages
	ccm->ccm_real.ct_threshold = cm_awb_list[1];
#endif
	memset(ccm->_ccm_a_parameter, 0, sizeof(ccm->_ccm_a_parameter));
	memset(ccm->_ccm_t_parameter, 0, sizeof(ccm->_ccm_t_parameter));
	memset(ccm->_ccm_d_parameter, 0, sizeof(ccm->_ccm_d_parameter));
	memset(ccm->cm_ev_list, 0, sizeof(ccm->cm_ev_list));
	memset(ccm->cm_sat_list, 0, sizeof(ccm->cm_sat_list));
	memset(ccm->cm_awb_list, 0, sizeof(ccm->cm_awb_list));

	tiziano_ccm_params_refresh(ccm);
	tiziano_ccm_lut_parameter(ccm, ccm->tiziano_ccm_d_linear);
	system_reg_write(((tisp_core_t *)ccm->core)->priv_data, CCM_ADDR_DP_CFG, ccm->tiziano_ccm_dp_cfg[0]);
	jz_isp_ccm_parameter_convert(ccm);
	memcpy(ccm->ccm_parameter, ccm->_ccm_d_parameter, sizeof(ccm->_ccm_d_parameter));

	return 0;
}

int32_t tisp_ccm_param_array_get(tisp_ccm_t *ccm, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_CCM_DP_CFG:
		len = sizeof(tparams.params_data.TISP_PARAM_CCM_DP_CFG);
		tmpbuf = ccm->tiziano_ccm_dp_cfg;
		break;
	case TISP_PARAM_CCM_A_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_CCM_A_LINEAR);
		tmpbuf = ccm->tiziano_ccm_a_linear;
		break;
	case TISP_PARAM_CCM_T_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_CCM_T_LINEAR);
		tmpbuf = ccm->tiziano_ccm_t_linear;
		break;
	case TISP_PARAM_CCM_D_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_CCM_D_LINEAR);
		tmpbuf = ccm->tiziano_ccm_d_linear;
		break;
	case TISP_PARAM_CCM_EV_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_CCM_EV_LIST);
		tmpbuf = ccm->cm_ev_list;
		break;
	case TISP_PARAM_CCM_SAT_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_CCM_SAT_LIST);
		tmpbuf = ccm->cm_sat_list;
		break;
	case TISP_PARAM_CCM_AWB_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_CCM_AWB_LIST);
		tmpbuf = ccm->cm_awb_list;
		break;
	default:
		ISP_ERROR("%s,%d: ccm not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	memcpy(buf, tmpbuf, len);
	*plen = len;

	return 0;
}

int32_t tisp_ccm_param_array_set(tisp_ccm_t *ccm, int32_t id, void *buf, uint32_t *plen)
{
	uint32_t len;
	void *tmpbuf;

	switch (id) {
	case TISP_PARAM_CCM_DP_CFG:
		len = sizeof(tparams.params_data.TISP_PARAM_CCM_DP_CFG);
		tmpbuf = ccm->tiziano_ccm_dp_cfg;
		break;
	case TISP_PARAM_CCM_A_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_CCM_A_LINEAR);
		tmpbuf = ccm->tiziano_ccm_a_linear;
		break;
	case TISP_PARAM_CCM_T_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_CCM_T_LINEAR);
		tmpbuf = ccm->tiziano_ccm_t_linear;
		break;
	case TISP_PARAM_CCM_D_LINEAR:
		len = sizeof(tparams.params_data.TISP_PARAM_CCM_D_LINEAR);
		tmpbuf = ccm->tiziano_ccm_d_linear;
		break;
	case TISP_PARAM_CCM_EV_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_CCM_EV_LIST);
		tmpbuf = ccm->cm_ev_list;
		break;
	case TISP_PARAM_CCM_SAT_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_CCM_SAT_LIST);
		tmpbuf = ccm->cm_sat_list;
		break;
	case TISP_PARAM_CCM_AWB_LIST:
		len = sizeof(tparams.params_data.TISP_PARAM_CCM_AWB_LIST);
		tmpbuf = ccm->cm_awb_list;
		break;
	default:
		ISP_ERROR("%s,%d: ccm not support param id %d\n", __func__, __LINE__, id);
		return -1;
	}

	memcpy(tmpbuf, buf, len);
	ccm->ccm_real.first_tune = 1;
	jz_isp_ccm(ccm);

	return 0;
}
