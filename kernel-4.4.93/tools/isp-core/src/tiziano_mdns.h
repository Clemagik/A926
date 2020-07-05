#ifndef __TIZIANO_MDNS_H__
#define __TIZIANO_MDNS_H__

#include "../inc/tiziano_core.h"

int32_t tiziano_mdns_init(tisp_mdns_t *mdns, uint32_t width, uint32_t height);
int32_t tisp_mdns_refresh(tisp_mdns_t *mdns, uint32_t gain_value);
int32_t tisp_mdns_param_array_set(tisp_mdns_t *mdns, int32_t id, void *buf, uint32_t *plen);
int32_t tisp_mdns_param_array_get(tisp_mdns_t *mdns, int32_t id, void *buf, uint32_t *plen);
int32_t tiziano_mdns_dn_params_refresh(tisp_mdns_t *mdns);

#endif
