#ifndef __TIZIANO_DEFOG_H__
#define __TIZIANO_DEFOG_H__

#include "../inc/tiziano_core.h"

int32_t tiziano_defog_init(tisp_defog_t *defog, uint32_t width, uint32_t height);
int32_t tisp_defog_param_array_set(tisp_defog_t *defog, int32_t id, void *buf, uint32_t *plen);
int32_t tisp_defog_param_array_get(tisp_defog_t *defog, int32_t id, void *buf, uint32_t *plen);
int32_t tisp_defog_ct_update(tisp_defog_t *defog, uint64_t data1);
int tiziano_defog_dn_params_refresh(tisp_defog_t *defog);
int32_t tisp_defog_ev_update(tisp_defog_t *defog, uint64_t data1);

void Tiziano_defog_fpga(Isp_Defog_Ct_Detect_Para TizianoDefogStructMe);

#endif
