#ifndef __TIZIANO_CLM_H__
#define __TIZIANO_CLM_H__

#include "../inc/tiziano_core.h"

int tiziano_clm_init(tisp_clm_t *clm);
int32_t tisp_clm_param_array_set(tisp_clm_t *clm, int32_t id, void *buf, uint32_t *plen);
int32_t tisp_clm_param_array_get(tisp_clm_t *clm, int32_t id, void *buf, uint32_t *plen);
int tiziano_clm_dn_params_refresh(tisp_clm_t *clm);

#endif
