#ifndef __TIZIANO_DMSC_H__
#define __TIZIANO_DMSC_H__

#include  "../inc/tiziano_core.h"

int32_t tiziano_dmsc_init(tisp_dmsc_t *dmsc);
int32_t tisp_dmsc_refresh(tisp_dmsc_t *dmsc, uint32_t gain_value);
int32_t tisp_dmsc_param_array_set(tisp_dmsc_t *dmsc, int32_t id, void *buf, uint32_t *plen);
int32_t tisp_dmsc_param_array_get(tisp_dmsc_t *dmsc, int32_t id, void *buf, uint32_t *plen);
int32_t tiziano_dmsc_dn_params_refresh(tisp_dmsc_t *dmsc);

#endif
