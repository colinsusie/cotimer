#include "timerwheel.h"

#define FIRST_INDEX(v) ((v) & TVR_MASK)
#define NTH_INDEX(v, n) (((v) >> (TVR_BITS + (n) * TVN_BITS)) & TVN_MASK)

void timerwheel_init(timerwheel_t *tw, uint16_t interval, uint64_t currtime) {
    memset(tw, 0, sizeof(*tw));
    tw->interval = interval;
    tw->lasttime = currtime;
    int i, j;
    for (i = 0; i < TVR_SIZE; ++i) {
        clinklist_init(tw->tvroot.vec + i);
    }
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < TVN_SIZE; ++j)
            clinklist_init(tw->tv[i].vec + j);
    }
}

void timerwheel_node_init(timernode_t *node, timer_cb_t cb, void *ud) {
    node->next = 0;
    node->prev = 0;
    node->userdata = ud;
    node->callback = cb;
    node->expire = 0;
}

static void _timerwheel_add(timerwheel_t *tw, timernode_t *node) {
    uint32_t expire = node->expire;
    uint32_t idx = expire - tw->currtick;
    clinknode_t *head;
    if (idx < TVR_SIZE) {
        head = tw->tvroot.vec + FIRST_INDEX(expire);
    } else {
        int i;
        uint64_t sz;
        for (i = 0; i < 4; ++i) {
            sz = (1ULL << (TVR_BITS + (i+1) * TVN_BITS));
            if (idx < sz) {
                idx = NTH_INDEX(expire, i);
                head = tw->tv[i].vec + idx;
                break;
            }
        }
    }
    clinklist_add_back(head, (clinknode_t*)node);
}

void timerwheel_add(timerwheel_t *tw, timernode_t *node, uint32_t ticks) {
    node->expire = tw->currtick + ((ticks > 0) ? ticks : 1);
    _timerwheel_add(tw, node);
}

int timerwheel_del(timerwheel_t *tw, timernode_t *node) {
    if (!clinklist_is_empty((clinknode_t*)node)) {
        clinklist_remote((clinknode_t*)node);
        return 1;
    }
    return 0;
}

void _timerwheel_cascade(timerwheel_t *tw, tvnum_t *tv, int idx) {
    clinknode_t head;
    clinklist_init(&head);
    clinklist_splice(tv->vec + idx, &head);
    while (!clinklist_is_empty(&head)) {
        timernode_t *node = (timernode_t*)head.next;
        clinklist_remote(head.next);
        _timerwheel_add(tw, node);
    }
}

void _timerwheel_tick(timerwheel_t *tw) {
    ++tw->currtick;

    uint32_t currtick = tw->currtick;
    int index = (currtick & TVR_MASK); 
    if (index == 0) {
        int i = 0;
        int idx;
        do {
            idx = NTH_INDEX(tw->currtick, i);
            _timerwheel_cascade(tw, &(tw->tv[i]), idx);
        } while (idx == 0 && ++i < 4);
    }

    clinknode_t head;
    clinklist_init(&head);
    clinklist_splice(tw->tvroot.vec + index, &head);
    while (!clinklist_is_empty(&head)) {
        timernode_t *node = (timernode_t*)head.next;
        clinklist_remote(head.next);
        if (node->callback) {
            node->callback(node->userdata);
        }
    }
}

void timerwheel_update(timerwheel_t *tw, uint64_t currtime) {
    if (currtime > tw->lasttime) {
        int diff = currtime - tw->lasttime + tw->remainder;
        int intv = tw->interval;
        tw->lasttime = currtime;
        while (diff >= intv) {
            diff -= intv;
            _timerwheel_tick(tw);
        }
        tw->remainder = diff;
    }
}