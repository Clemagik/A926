#ifndef __TIZIANO_ADR_H__
#define __TIZIANO_ADR_H__

#include "../inc/tiziano_core.h"


int32_t tiziano_adr_init(tisp_adr_t *adr, uint32_t width, uint32_t height);
int32_t tisp_adr_gain_interpolation(tisp_adr_t *adr, uint32_t ag_value);
int32_t tisp_adr_param_array_set(tisp_adr_t *adr, int32_t id, void *buf, uint32_t *plen);
int32_t tisp_adr_param_array_get(tisp_adr_t *adr, int32_t id, void *buf, uint32_t *plen);
void Tiziano_adr_fpga(Isp_Adr_Ct_Detect_Para TizianoAdrFpgaStructMe);
int32_t tiziano_adr_dn_params_refresh(tisp_adr_t *adr);
int32_t tisp_adr_ev_update(tisp_adr_t *adr, uint64_t data1);

#endif
