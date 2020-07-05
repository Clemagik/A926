#ifndef __TIZIANO_GAMMA_H__
#define __TIZIANO_GAMMA_H__

#include "../inc/tiziano_core.h"

int tiziano_gamma_init(tisp_gamma_t *gamma);
int32_t tisp_gamma_param_array_set(tisp_gamma_t *gamma, int32_t id, void *buf, uint32_t *plen);
int32_t tisp_gamma_param_array_get(tisp_gamma_t *gamma, int32_t id, void *buf, uint32_t *plen);
int tiziano_gamma_dn_params_refresh(tisp_gamma_t *gamma);

#endif
