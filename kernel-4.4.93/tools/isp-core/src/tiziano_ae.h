#ifndef __TIZIANO_AE_H__
#define __TIZIANO_AE_H__

#include "../inc/tiziano_core.h"

int32_t tiziano_ae_init(tisp_ae_t *ae, uint32_t height,uint32_t width,tisp_ae_ex_min_t ae_init);
int32_t tisp_ae_param_array_set(tisp_ae_t *ae, int32_t id, void *buf, uint32_t *plen);
int32_t tisp_ae_param_array_get(tisp_ae_t *ae, int32_t id, void *buf, uint32_t *plen);
int tiziano_ae_dn_params_refresh(tisp_ae_t *ae);
void tiziano_deflicker_expt_tune(tisp_ae_t *ae, uint32_t pal, uint32_t fps, uint32_t vts, uint32_t hts);
int tisp_ae_manual_set(tisp_ae_t *ae, tisp_ae_ctrls_t ae_manual);
int tisp_ae_get_hist_custome(tisp_ae_t *ae, tisp_ae_sta_t *ae_hist);
int tisp_ae_set_hist_custome(tisp_ae_t *ae, tisp_ae_sta_t ae_hist);
int tisp_ae_g_min(tisp_ae_t *ae, tisp_ae_ex_min_t *ae_min);
int tisp_ae_s_min(tisp_ae_t *ae, tisp_ae_ex_min_t ae_min);
int tisp_ae_get_y_zone(tisp_ae_t *ae, tisp_zone_info_t *ae_zone);
int tiziano_ae_compensation_set(tisp_ae_t *ae, uint32_t data);
int tiziano_ae_set_hardware_param(tisp_ae_t *ae);
void system_reg_write_ae(tisp_ae_t *ae, uint32_t trig, uint32_t addr, uint32_t value);
void tiziano_ae_s_ev_start(tisp_ae_t *ae, unsigned int ev_start);
int tiziano_ae_s_max_again(tisp_ae_t *ae, unsigned int max_again);
int tiziano_ae_s_max_isp_dgain(tisp_ae_t *ae, unsigned int max_isp_dgain);
#endif
