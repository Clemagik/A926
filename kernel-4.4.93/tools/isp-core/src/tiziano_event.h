#ifndef __TIZIANO_EVENT_H__
#define __TIZIANO_EVENT_H__

#include <linux/list.h>
#include "../inc/tiziano_core.h"


int32_t tisp_event_init(tisp_event_mg_t *tisp_event_mg);
int32_t tisp_event_push(tisp_event_mg_t *tisp_event_mg, tisp_event_t *ev);
int32_t tisp_event_exit(tisp_event_mg_t *tisp_event_mg);
int32_t tisp_event_set_cb(tisp_event_mg_t *tisp_event_mg, int id, event_process_cb cb_func, void *cb_data);
int32_t tisp_event_process(tisp_event_mg_t *tisp_event_mg);

#endif
