#ifndef __TIZIANO_HLDC_H__
#define __TIZIANO_HLDC_H__


#include "../inc/tiziano_core.h"

int32_t tiziano_hldc_init(tisp_hldc_t *hldc);
int32_t tisp_hldc_param_array_set(tisp_hldc_t *hldc, int32_t id, void *buf, uint32_t *plen);
int32_t tisp_hldc_param_array_get(tisp_hldc_t *hldc, int32_t id, void *buf, uint32_t *plen);

#endif
