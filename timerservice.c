#include "timerservice.h"
#include <stdint.h>
#include <string.h>

typedef struct timeritem {
	uint32_t loop;
    timerservice_t *sv;
	void *ud;
    timer_cb_t cb;
	timernode_t node;
} timeritem_t;

timerservice_t* ts_init(uint16_t interval, uint64_t currtime) {
    timerservice_t* sv = malloc(sizeof(timerservice_t));
    fallocator_init(&sv->alloc, 4096, sizeof(timeritem_t));
    timerwheel_init(&sv->twheel, interval, currtime);
    return sv;
}

void ts_free(timerservice_t* sv) {
    fallocator_free(&sv->alloc);
    free(sv);
}

void _on_timer(void *ud) {
    timeritem_t *item = ud;
    timerservice_t *sv = item->sv;
    if (item->loop) {
        timerwheel_add(&sv->twheel, &item->node, item->loop);
    }
    if (item->cb) {
        item->cb(item->ud);
    }
}

void* ts_add_timer(timerservice_t *sv, void *group, uint32_t delay, uint32_t loop, timer_cb_t cb, void *ud) {
    timeritem_t *item =  fallocator_newitem(&sv->alloc);
    item->sv = sv;
    item->loop = loop;
    item->cb = cb;
    item->ud = ud;
    timerwheel_node_init(&item->node, _on_timer, item);
    timerwheel_add(&sv->twheel, &item->node, delay);
    return item;
}

void ts_del_timer(timerservice_t *sv, void *group, void **handle) {
    if (!handle || !*handle)
        return;
    if (!fallocator_isfree(&sv->alloc, *handle)) {
        timeritem_t *item = *handle;
        timerwheel_del(&sv->twheel, &item->node);
        fallocator_freeitem(&sv->alloc, item);
        *handle = NULL;
    }
}

void ts_update(timerservice_t *sv, uint64_t currtime) {
    timerwheel_update(&sv->twheel, currtime);
}