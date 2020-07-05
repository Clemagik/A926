#ifndef __TIZIANO_AWB_H__
#define __TIZIANO_AWB_H__

#include "../inc/tiziano_core.h"


void Tiziano_Awb_Ct_Detect(uint32_t *zone_rgbg, unsigned int *zone_pix_cnt,
			   ISP_AWB_CT_DETECT_PARAM IspAwbCtDetectParam,
			   unsigned int *awb_gain, unsigned int *awb_fail);

int tiziano_awb_init(tisp_awb_t *awb, uint32_t height,uint32_t width);
int tisp_awb_ev_update(tisp_awb_t *awb, uint64_t data1);
int32_t tisp_awb_param_array_set(tisp_awb_t *awb, int32_t id, void *buf, uint32_t *plen);
int32_t tisp_awb_param_array_get(tisp_awb_t *awb, int32_t id, void *buf, uint32_t *plen);
int tiziano_awb_dn_params_refresh(tisp_awb_t *awb);
int tisp_g_wb_mode(tisp_awb_t *awb, tisp_wb_attr_t *wb_attr);
int tisp_s_wb_mode(tisp_awb_t *awb, tisp_wb_attr_t wb_attr);
void tisp_awb_set_frz(tisp_awb_t *awb, unsigned char frz);
void tisp_awb_get_frz(tisp_awb_t *awb, unsigned char *frz);
#endif
