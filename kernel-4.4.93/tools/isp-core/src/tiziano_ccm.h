#ifndef __TIZIANO_CCM_H__
#define __TIZIANO_CCM_H__

#include "../inc/tiziano_core.h"

uint32_t tiziano_ccm_init(tisp_ccm_t *ccm);
int32_t tisp_ccm_param_array_set(tisp_ccm_t *ccm, int32_t id, void *buf, uint32_t *plen);
int32_t tisp_ccm_param_array_get(tisp_ccm_t *ccm, int32_t id, void *buf, uint32_t *plen);
int32_t tisp_ccm_ev_update(tisp_ccm_t *ccm, uint64_t data1);
int32_t tisp_ccm_ct_update(tisp_ccm_t *ccm, uint64_t data1);
uint32_t tiziano_ccm_dn_params_refresh(tisp_ccm_t *ccm);

#endif
