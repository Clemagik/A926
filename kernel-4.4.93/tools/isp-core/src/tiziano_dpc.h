#ifndef __TIZIANO_DPC_H__
#define __TIZIANO_DPC_H__

#include "../inc/tiziano_core.h"

int32_t tiziano_dpc_init(tisp_dpc_t *dpc);
int32_t tisp_dpc_refresh(tisp_dpc_t *dpc, uint32_t gain_value);
int32_t tisp_dpc_param_array_set(tisp_dpc_t *dpc, int32_t id, void *buf, uint32_t *plen);
int32_t tisp_dpc_param_array_get(tisp_dpc_t *dpc, int32_t id, void *buf, uint32_t *plen);
int32_t tiziano_dpc_dn_params_refresh(tisp_dpc_t *dpc);

#endif
