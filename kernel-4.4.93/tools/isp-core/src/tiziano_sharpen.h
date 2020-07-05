#ifndef __TIZIANO_SHARPEN_H__
#define __TIZIANO_SHARPEN_H__

#include "../inc/tiziano_core.h"

int32_t tiziano_sharpen_init(tisp_sharpen_t *sharpen);
int32_t tisp_sharpen_refresh(tisp_sharpen_t *sharpen, uint32_t gain_value);
int32_t tisp_sharpen_param_array_set(tisp_sharpen_t *sharpen, int32_t id, void *buf, uint32_t *plen);
int32_t tisp_sharpen_param_array_get(tisp_sharpen_t *sharpen, int32_t id, void *buf, uint32_t *plen);
int32_t tiziano_sharpen_dn_params_refresh(tisp_sharpen_t *sharpen);

#endif
