#ifndef __TIZIANO_AF_H__
#define __TIZIANO_AF_H__

#include "../inc/tiziano_core.h"

int32_t tiziano_af_init(tisp_af_t *af, uint32_t width, uint32_t height);
int32_t tisp_af_param_array_set(tisp_af_t *af, int32_t id, void *buf, uint32_t *plen);
int32_t tisp_af_param_array_get(tisp_af_t *af, int32_t id, void *buf, uint32_t *plen);
int tisp_af_get_metric(tisp_af_t *af, unsigned int *metric);
int tisp_af_get_attr(tisp_af_t *af, tisp_af_attr *af_info);
int tisp_af_set_attr(tisp_af_t *af, tisp_af_attr af_info);
void tiziano_af_dn_params_refresh(tisp_af_t *af);

#endif
