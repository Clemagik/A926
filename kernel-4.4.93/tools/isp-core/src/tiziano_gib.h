#ifndef __TIZIANO_GIB_H__
#define __TIZIANO_GIB_H__

#include "../inc/tiziano_core.h"

int tiziano_gib_init(tisp_gib_t *gib);
int32_t tisp_gib_gain_interpolation(tisp_gib_t *gib, uint32_t ag_value);
int32_t tisp_gib_param_array_set(tisp_gib_t *gib, int32_t id, void *buf, uint32_t *plen);
int32_t tisp_gib_param_array_get(tisp_gib_t *gib, int32_t id, void *buf, uint32_t *plen);
int tiziano_gib_dn_params_refresh(tisp_gib_t *gib);
int32_t tisp_gib_deir_ir_update(tisp_gib_t *gib, uint64_t data1);
void DeirEn_DayNightCut(tisp_gib_t *gib, uint32_t DayNightCut);

extern uint32_t tiziano_gib_config_line[6];

#endif
