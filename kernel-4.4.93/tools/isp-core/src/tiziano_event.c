#include<linux/string.h>
#include<linux/kernel.h>
#include<linux/delay.h>
#include<linux/slab.h>
#include "isp-debug.h"

#include "tiziano_event.h"


int tisp_event_init(tisp_event_mg_t *tisp_event_mg)
{
	int i = 0;
	//event list init
	INIT_LIST_HEAD(&tisp_event_mg->tevent_info.event_working);
	INIT_LIST_HEAD(&tisp_event_mg->tevent_info.event_free);
	for (i = 0; i < TISP_PROCESS_EVENT_NUM; i++) {
		INIT_LIST_HEAD(&tisp_event_mg->tevent_info.events[i].entry);
	}
	for (i = 0; i < TISP_PROCESS_EVENT_NUM; i++) {
		list_add_tail(&tisp_event_mg->tevent_info.events[i].entry, &tisp_event_mg->tevent_info.event_free);
	}
	init_completion(&tisp_event_mg->tevent_info.event_compl);

	return 0;
}

int tisp_event_set_cb(tisp_event_mg_t *tisp_event_mg, int id, event_process_cb cb_func, void *cb_data)
{
	tisp_event_mg->cb[id] = cb_func;
	tisp_event_mg->cb_data[id] = cb_data;

	return 0;
}

int tisp_event_push(tisp_event_mg_t *tisp_event_mg, tisp_event_t *ev)
{
	int ret = 0;
	unsigned long flags;
	tisp_event_t *event;
	spin_lock_irqsave(&tisp_event_mg->tevent_info.event_slock, flags);
	if (list_empty(&tisp_event_mg->tevent_info.event_free)) {
		ISP_ERROR("%s,%d: event free empty error\n", __func__, __LINE__);
		ret = -1;
		goto err_event_free_empty;
	}
	event = list_first_entry(&tisp_event_mg->tevent_info.event_free, tisp_event_t, entry);
	list_del(&event->entry);
	event->type = ev->type;
	event->data1 = ev->data1;
	event->data2 = ev->data2;
	event->data3 = ev->data3;
	event->data4 = ev->data4;
	list_add_tail(&event->entry, &tisp_event_mg->tevent_info.event_working);
	complete(&tisp_event_mg->tevent_info.event_compl);
	spin_unlock_irqrestore(&tisp_event_mg->tevent_info.event_slock, flags);

	return 0;
err_event_free_empty:
	spin_unlock_irqrestore(&tisp_event_mg->tevent_info.event_slock, flags);

	return ret;
}

int tisp_event_exit(tisp_event_mg_t *tisp_event_mg)
{
	int ret = 0;
	tisp_event_t event;

	event.type = TISP_EVENT_TYPE_EXIT;
	tisp_event_push(tisp_event_mg, &event);

	return ret;
}

int tisp_event_process(tisp_event_mg_t *tisp_event_mg)
{
	int ret = 0;
	unsigned long flags;
	tisp_event_t *event;
	tisp_event_t event_local = {0};

	ret = wait_for_completion_timeout(&tisp_event_mg->tevent_info.event_compl, 20);
	if (-ERESTARTSYS == ret) {
		ISP_ERROR("%s,%d: wake up by signal\n", __func__, __LINE__);
		return 0;
	} else if (0 == ret) {
		//ISP_ERROR("%s,%d: timeout\n", __func__, __LINE__);
		return 0;
	}
	spin_lock_irqsave(&tisp_event_mg->tevent_info.event_slock, flags);
	if (list_empty(&tisp_event_mg->tevent_info.event_working)) {
		ISP_ERROR("%s,%d: error\n", __func__, __LINE__);
		ret = -1;
		goto err_event_working_empty;
	}
	event = list_first_entry(&tisp_event_mg->tevent_info.event_working, tisp_event_t, entry);
	list_del(&event->entry);

	event_local = *event;

	list_add_tail(&event->entry, &tisp_event_mg->tevent_info.event_free);
	spin_unlock_irqrestore(&tisp_event_mg->tevent_info.event_slock, flags);

	if (TISP_EVENT_TYPE_EXIT == tisp_event_mg->cb[event_local.type]) {
	} else if(tisp_event_mg->cb[event_local.type]) {
		tisp_event_mg->cb[event_local.type](tisp_event_mg->cb_data[event_local.type], event_local.data1, event_local.data2, event_local.data3, event_local.data4);
	} else {
		//ISP_ERROR("%s,%d: error event_local cb func , type = %d\n", __func__, __LINE__, event_local.type);
	}

	return 0;

err_event_working_empty:
	spin_unlock_irqrestore(&tisp_event_mg->tevent_info.event_slock, flags);
	return ret;
}
