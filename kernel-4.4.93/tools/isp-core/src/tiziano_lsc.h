#ifndef __TIZIANO_LSC_H__
#define __TIZIANO_LSC_H__

#include "../inc/tiziano_core.h"

int tiziano_lsc_init(tisp_lsc_t *lsc);
int32_t tisp_lsc_param_array_set(tisp_lsc_t *lsc, int32_t id, void *buf, uint32_t *plen);
int32_t tisp_lsc_param_array_get(tisp_lsc_t *lsc, int32_t id, void *buf, uint32_t *plen);
int32_t tisp_lsc_ct_update(tisp_lsc_t *lsc, uint64_t data1);
int32_t tisp_lsc_gain_update(tisp_lsc_t *lsc, uint64_t data1);
int tiziano_lsc_dn_params_refresh(tisp_lsc_t *lsc);

#endif
