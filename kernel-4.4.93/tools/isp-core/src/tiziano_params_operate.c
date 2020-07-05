#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include "isp-debug.h"

#include "tiziano_map.h"
#include "tiziano_params.h"
#include "tiziano_netlink.h"
#include "../inc/tiziano_isp.h"
#include "tiziano_core.h"
#include "tiziano_ae.h"
#include "tiziano_awb.h"
#include "tiziano_gamma.h"
#include "tiziano_gib.h"
#include "tiziano_lsc.h"
#include "tiziano_dmsc.h"
#include "tiziano_ccm.h"
#include "tiziano_sharpen.h"
#include "tiziano_dpc.h"
#include "tiziano_sdns.h"
#include "tiziano_mdns.h"
#include "tiziano_clm.h"
#include "tiziano_defog.h"
#include "tiziano_adr.h"
#include "tiziano_hldc.h"
#include "tiziano_af.h"
#include "tiziano_params_operate.h"
#include "../inc/tiziano_core_tuning.h"

#define TISP_PARAM_OP_BUF_SIZE (64*1024)
#define DEBUG //ISP_ERROR("###%s::%d###\n",__func__,__LINE__)
//static tisp_param_op_msg_t * opmsg = NULL;


int32_t tisp_param_operate_process(void *cb_data, void *data, uint32_t len)
{
	uint32_t msg_type;
	tisp_param_operate_t *param_operate = (tisp_param_operate_t *)cb_data;
	tisp_core_t *core = param_operate->core;
	tisp_ae_t *ae = &core->ae;
	tisp_param_op_msg_t *rmsg = NULL;
	uint32_t array_id = 0;
	uint32_t tmplen = 0;
	uint32_t ctrl_id = 0;
	uint32_t *ptr = NULL;

	rmsg = data;
	msg_type = rmsg->msg_type;
	if (TISP_PARAM_OP_TYPE_ARRAY_GET == rmsg->msg_type) {
		DEBUG;
		array_id = rmsg->msg_data1;
		tmplen = 0;
		if ((array_id>=TISP_PARAM_TOP_START)&&(array_id<=TISP_PARAM_TOP_END)) {
			tisp_ae_param_array_get(&core->ae, array_id, param_operate->opmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_AE_START)&&(array_id<=TISP_PARAM_AE_END)) {
			tisp_ae_param_array_get(&core->ae, array_id, param_operate->opmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_AWB_START)&&(array_id<=TISP_PARAM_AWB_END)) {
			tisp_awb_param_array_get(&core->awb, array_id,param_operate->opmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_GAMMA_START)&&(array_id<=TISP_PARAM_GAMMA_END)) {
			tisp_gamma_param_array_get(&core->gamma, array_id,param_operate->opmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_GIB_START)&&(array_id<=TISP_PARAM_GIB_END)) {
			tisp_gib_param_array_get(&core->gib, array_id,param_operate->opmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_DMSC_START)&&(array_id<=TISP_PARAM_DMSC_END)) {
			tisp_dmsc_param_array_get(&core->dmsc, array_id,param_operate->opmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_LSC_START)&&(array_id<=TISP_PARAM_LSC_END)) {
			tisp_lsc_param_array_get(&core->lsc, array_id,param_operate->opmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_CCM_START)&&(array_id<=TISP_PARAM_CCM_END)) {
			tisp_ccm_param_array_get(&core->ccm, array_id,param_operate->opmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_SHARPEN_START)&&(array_id<=TISP_PARAM_SHARPEN_END)) {
			tisp_sharpen_param_array_get(&core->sharpen, array_id,param_operate->opmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_DPC_START)&&(array_id<=TISP_PARAM_DPC_END)) {
			tisp_dpc_param_array_get(&core->dpc, array_id,param_operate->opmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_SDNS_START)&&(array_id<=TISP_PARAM_SDNS_END)) {
			tisp_sdns_param_array_get(&core->sdns, array_id,param_operate->opmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_MDNS_START)&&(array_id<=TISP_PARAM_MDNS_END)) {
			tisp_mdns_param_array_get(&core->mdns, array_id,param_operate->opmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_CLM_START)&&(array_id<=TISP_PARAM_CLM_END)) {
			tisp_clm_param_array_get(&core->clm, array_id,param_operate->opmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_DEFOG_START)&&(array_id<=TISP_PARAM_DEFOG_END)) {
			tisp_defog_param_array_get(&core->defog, array_id,param_operate->opmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_ADR_START)&&(array_id<=TISP_PARAM_ADR_END)) {
			tisp_adr_param_array_get(&core->adr, array_id,param_operate->opmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_HLDC_START)&&(array_id<=TISP_PARAM_HLDC_END)) {
			tisp_hldc_param_array_get(&core->hldc, array_id,param_operate->opmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_AF_START)&&(array_id<=TISP_PARAM_AF_END)) {
			tisp_af_param_array_get(&core->af, array_id,param_operate->opmsg->msg_buf, &tmplen);
		} else {
			ISP_ERROR("%s,%d: unsupport msg arraty id\n", __func__, __LINE__);
			return -1;
		}
		param_operate->opmsg->msg_type = rmsg->msg_type;
		param_operate->opmsg->msg_data1 = rmsg->msg_data1;
		param_operate->opmsg->msg_data2 = rmsg->msg_data2;
		param_operate->opmsg->msg_data3 = rmsg->msg_data3;
		param_operate->opmsg->msg_data4 = rmsg->msg_data4;
		netlink_send_msg(&param_operate->nl, (void*)param_operate->opmsg, sizeof(tisp_param_op_msg_t)+tmplen);
	} else if (TISP_PARAM_OP_TYPE_ARRAY_SET == rmsg->msg_type) {
		DEBUG;
		array_id = rmsg->msg_data1;
		tmplen = rmsg->msg_data2;
		if ((array_id>=TISP_PARAM_TOP_START)&&(array_id<=TISP_PARAM_TOP_END)) {
			tisp_ae_param_array_set(&core->ae, array_id, rmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_AE_START)&&(array_id<=TISP_PARAM_AE_END)) {
			tisp_ae_param_array_set(&core->ae, array_id, rmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_AWB_START)&&(array_id<=TISP_PARAM_AWB_END)) {
			tisp_awb_param_array_set(&core->awb, array_id, rmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_GAMMA_START)&&(array_id<=TISP_PARAM_GAMMA_END)) {
			tisp_gamma_param_array_set(&core->gamma, array_id, rmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_GIB_START)&&(array_id<=TISP_PARAM_GIB_END)) {
			tisp_gib_param_array_set(&core->gib, array_id, rmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_LSC_START)&&(array_id<=TISP_PARAM_LSC_END)) {
			tisp_lsc_param_array_set(&core->lsc, array_id, rmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_DMSC_START)&&(array_id<=TISP_PARAM_DMSC_END)) {
			tisp_dmsc_param_array_set(&core->dmsc, array_id, rmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_CCM_START)&&(array_id<=TISP_PARAM_CCM_END)) {
			tisp_ccm_param_array_set(&core->ccm, array_id, rmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_SHARPEN_START)&&(array_id<=TISP_PARAM_SHARPEN_END)) {
			tisp_sharpen_param_array_set(&core->sharpen, array_id, rmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_DPC_START)&&(array_id<=TISP_PARAM_DPC_END)) {
			tisp_dpc_param_array_set(&core->dpc, array_id, rmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_SDNS_START)&&(array_id<=TISP_PARAM_SDNS_END)) {
			tisp_sdns_param_array_set(&core->sdns, array_id, rmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_MDNS_START)&&(array_id<=TISP_PARAM_MDNS_END)) {
			tisp_mdns_param_array_set(&core->mdns, array_id, rmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_CLM_START)&&(array_id<=TISP_PARAM_CLM_END)) {
			tisp_clm_param_array_set(&core->clm, array_id, rmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_DEFOG_START)&&(array_id<=TISP_PARAM_DEFOG_END)) {
			tisp_defog_param_array_set(&core->defog, array_id, rmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_ADR_START)&&(array_id<=TISP_PARAM_ADR_END)) {
			tisp_adr_param_array_set(&core->adr, array_id, rmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_HLDC_START)&&(array_id<=TISP_PARAM_HLDC_END)) {
			tisp_hldc_param_array_set(&core->hldc, array_id,rmsg->msg_buf, &tmplen);
		} else if ((array_id>=TISP_PARAM_AF_START)&&(array_id<=TISP_PARAM_AF_END)) {
			tisp_af_param_array_set(&core->af, array_id,rmsg->msg_buf, &tmplen);
		} else {
			ISP_ERROR("%s,%d: unsupport msg arraty id\n", __func__, __LINE__);
			return -1;
		}
		netlink_send_msg(&param_operate->nl, (void*)param_operate->opmsg, sizeof(tisp_param_op_msg_t));
	} else if (TISP_PARAM_OP_CTRLS_SET == rmsg->msg_type) {
		DEBUG;
		ctrl_id = rmsg->msg_data1;
		ptr = (uint32_t*)rmsg->msg_buf;

		param_operate->opmsg->msg_type = rmsg->msg_type;
		param_operate->opmsg->msg_data1 = rmsg->msg_data1;
		param_operate->opmsg->msg_data2 = rmsg->msg_data2;
		param_operate->opmsg->msg_data3 = rmsg->msg_data3;
		param_operate->opmsg->msg_data4 = rmsg->msg_data4;
		param_operate->opmsg->msg_ret = 0;
		if (TISP_AE_GENERAL_CTRLS == ctrl_id) {
			tisp_ae_ctrls_t tisp_ae_ctrls_set_para;
			DEBUG;
			memcpy(&tisp_ae_ctrls_set_para, (void*)ptr, sizeof(tisp_ae_ctrls_t));
			ae->tisp_ae_ctrls.tisp_ae_manual = tisp_ae_ctrls_set_para.tisp_ae_manual;
			ae->tisp_ae_ctrls.tisp_ae_sensor_again = tisp_ae_ctrls_set_para.tisp_ae_sensor_again;
			ae->tisp_ae_ctrls.tisp_ae_sensor_dgain = tisp_ae_ctrls_set_para.tisp_ae_sensor_dgain;
			ae->tisp_ae_ctrls.tisp_ae_sensor_integration_time =
				tisp_ae_ctrls_set_para.tisp_ae_sensor_integration_time;
			ae->tisp_ae_ctrls.tisp_ae_isp_dgian = tisp_ae_ctrls_set_para.tisp_ae_isp_dgian;
			ae->tisp_ae_ctrls.tisp_ae_max_sensor_again =
				tisp_ae_ctrls_set_para.tisp_ae_max_sensor_again;
			ae->tisp_ae_ctrls.tisp_ae_max_sensor_dgain =
				tisp_ae_ctrls_set_para.tisp_ae_max_sensor_dgain;
			ae->tisp_ae_ctrls.tisp_ae_max_sensor_integration_time =
				tisp_ae_ctrls_set_para.tisp_ae_max_sensor_integration_time;
			ae->tisp_ae_ctrls.tisp_ae_max_isp_dgain = tisp_ae_ctrls_set_para.tisp_ae_max_isp_dgain;
			ae->tisp_ae_ctrls.tisp_ae_ir = tisp_ae_ctrls_set_para.tisp_ae_ir;
			ae->tisp_ae_ctrls.tisp_ae_it_manual = tisp_ae_ctrls_set_para.tisp_ae_it_manual;
			ae->tisp_ae_ctrls.tisp_ae_ag_manual = tisp_ae_ctrls_set_para.tisp_ae_ag_manual;
			ae->tisp_ae_ctrls.tisp_ae_dg_manual = tisp_ae_ctrls_set_para.tisp_ae_dg_manual;
		} else if (TISP_TOP_CTRLS == ctrl_id) {
			uint32_t bypass_reg = rmsg->msg_data2;
			DEBUG;
			system_reg_write(((tisp_core_t *)param_operate->core)->priv_data, TIZAINO_BYPASS_CON, bypass_reg);
		} else if (TISP_REG_CTRLS == ctrl_id) {
			uint32_t reg = rmsg->msg_data2;
			uint32_t value = rmsg->msg_data3;
			DEBUG;
			system_reg_write(((tisp_core_t *)param_operate->core)->priv_data, TIZIANO_BASE + (reg & 0x1ffff), value);
		} else if (TISP_DAY_NIGHT_CTRLS == ctrl_id) {
			uint32_t value = rmsg->msg_data2;
			TISP_MODE_DN_E dn = TISP_MODE_DAY_MODE;
			DEBUG;
			if(value == 0)
				dn = TISP_MODE_DAY_MODE;
			else if (value == 1)
				dn = TISP_MODE_NIGHT_MODE;
			else
				ISP_ERROR("%s:%d::Cant support this mode!!!\n",__func__,__LINE__);
			tisp_day_or_night_s_ctrl(&core->core_tuning, dn);
		} else if (TISP_AWB_CTRLS == ctrl_id) {
			tisp_wb_attr_t wb_attr;
			DEBUG;
			memcpy(&wb_attr, (void*)ptr, sizeof(tisp_wb_attr_t));
			tisp_s_wb_attr(&core->core_tuning, wb_attr);
		} else {
			ISP_ERROR("%s,%d: unsupport msg arraty id\n", __func__, __LINE__);
			param_operate->opmsg->msg_ret = 1;
		}
		netlink_send_msg(&param_operate->nl, (void*)param_operate->opmsg, sizeof(tisp_param_op_msg_t));
	} else if (TISP_PARAM_OP_CTRLS_GET == rmsg->msg_type) {
		DEBUG;
		ctrl_id = rmsg->msg_data1;
		ptr = (uint32_t*)param_operate->opmsg->msg_buf;
		param_operate->opmsg->msg_type = rmsg->msg_type;
		param_operate->opmsg->msg_data1 = rmsg->msg_data1;
		param_operate->opmsg->msg_data2 = rmsg->msg_data2;
		param_operate->opmsg->msg_data3 = rmsg->msg_data3;
		param_operate->opmsg->msg_data4 = rmsg->msg_data4;
		param_operate->opmsg->msg_ret = 0;
		if (TISP_AE_GENERAL_CTRLS == ctrl_id) {
			DEBUG;
			memcpy((void*)ptr, &ae->tisp_ae_ctrls, sizeof(ae->tisp_ae_ctrls));
			netlink_send_msg(&param_operate->nl, (void*)param_operate->opmsg, sizeof(tisp_param_op_msg_t)+sizeof(ae->tisp_ae_ctrls));
		} else if (TISP_TOP_CTRLS == ctrl_id) {
			uint32_t bypass_reg = 0;
			DEBUG;
			bypass_reg = system_reg_read(((tisp_core_t *)param_operate->core)->priv_data, TIZAINO_BYPASS_CON);
			param_operate->opmsg->msg_data2 = bypass_reg;
			netlink_send_msg(&param_operate->nl, (void*)param_operate->opmsg, sizeof(tisp_param_op_msg_t));
		} else if (TISP_REG_CTRLS == ctrl_id) {
			uint32_t value = 0;
			uint32_t reg = rmsg->msg_data2;
			DEBUG;
			value = system_reg_read(((tisp_core_t *)param_operate->core)->priv_data, TIZIANO_BASE + (reg & 0x1ffff));
			param_operate->opmsg->msg_data3 = value;
			netlink_send_msg(&param_operate->nl, (void*)param_operate->opmsg, sizeof(tisp_param_op_msg_t));
		} else if (TISP_DAY_NIGHT_CTRLS == ctrl_id) {
			uint32_t value = 0;
			TISP_MODE_DN_E dn;
			DEBUG;
			dn = tisp_day_or_night_g_ctrl(&core->core_tuning);
			if(dn == TISP_MODE_DAY_MODE)
				value = 0;
			else if (dn == TISP_MODE_NIGHT_MODE)
				value = 1;
			else
				ISP_ERROR("Have no Value\n");
			param_operate->opmsg->msg_data3 = value;
			netlink_send_msg(&param_operate->nl, (void*)param_operate->opmsg, sizeof(tisp_param_op_msg_t));
		} else if (TISP_SENSOR_INFO_CTRLS == ctrl_id) {
			DEBUG;
			memcpy((void*)ptr, &core->sensor_info, sizeof(core->sensor_info));
			netlink_send_msg(&param_operate->nl, (void*)param_operate->opmsg, sizeof(tisp_param_op_msg_t)+sizeof(core->sensor_info));
		} else if (TISP_AWB_CTRLS == ctrl_id) {
			tisp_wb_attr_t wb_attr;
			DEBUG;
			tisp_g_wb_attr(&core->core_tuning, &wb_attr);
			memcpy((void*)ptr, &wb_attr, sizeof(tisp_wb_attr_t));
			netlink_send_msg(&param_operate->nl, (void*)param_operate->opmsg, sizeof(tisp_param_op_msg_t)+sizeof(tisp_wb_attr_t));
		} else {
			DEBUG;
			ISP_ERROR("%s,%d: unsupport msg arraty id\n", __func__, __LINE__);
			param_operate->opmsg->msg_ret = 1;
		}
	} else {
		DEBUG;
		ISP_ERROR("%s,%d: unsupport msg type\n", __func__, __LINE__);
		return -1;
	}

	return 0;
}

int32_t tisp_param_operate_init(tisp_param_operate_t *param_operate)
{
	int32_t ret = 0;
	param_operate->opmsg = kmalloc(sizeof(tisp_param_op_msg_t)+TISP_PARAM_OP_BUF_SIZE, GFP_KERNEL);
	if (NULL == param_operate->opmsg) {
		ISP_ERROR("%s,%d: kmalloc error\n", __func__, __LINE__);
		return -1;
	}
	tisp_netlink_init(&param_operate->nl);

	tisp_netlink_event_set_cb(&param_operate->nl, tisp_param_operate_process, param_operate);

	return ret;
}

int32_t tisp_param_operate_deinit(tisp_param_operate_t *param_operate)
{
	tisp_netlink_exit(&param_operate->nl);
	if (NULL != param_operate->opmsg) {
	    kfree(param_operate->opmsg);
		param_operate->opmsg = NULL;
	}
	return 0;
}
