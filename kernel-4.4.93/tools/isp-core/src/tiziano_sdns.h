#ifndef __TIZIANO_SDNS_H__
#define __TIZIANO_SDNS_H__

#include "../inc/tiziano_core.h"

int32_t tiziano_sdns_init(tisp_sdns_t *sdns);
int32_t tisp_sdns_refresh(tisp_sdns_t *sdns, uint32_t gain_value);
int32_t tisp_sdns_param_array_set(tisp_sdns_t *sdns, int32_t id, void *buf, uint32_t *plen);
int32_t tisp_sdns_param_array_get(tisp_sdns_t *sdns, int32_t id, void *buf, uint32_t *plen);
int32_t tiziano_sdns_dn_params_refresh(tisp_sdns_t *sdns);

#endif
