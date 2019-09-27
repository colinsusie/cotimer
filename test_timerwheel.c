#include "timerwheel.h"
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

static timerwheel_t tw;
static timernode_t node;
static timernode_t node2;
static timernode_t node3;
static timernode_t node4;

static uint64_t gettime() {
    struct timeval tm;
    gettimeofday(&tm, NULL);
    return (tm.tv_sec * 1000 + tm.tv_usec / 1000);
}

void _print_timerwheel(clinknode_t *vec, int idx, int size) {
    int i;
    printf("========================================%d\n", idx);
    for (i = 0;i < size; ++i) {
        clinknode_t *head = vec + i;
        if (!clinklist_is_empty(head)) {
            clinknode_t *node = head->next;
            while (node != head) {
                timernode_t *tnode = (timernode_t*)node;
                printf("  %d: (expire=%u)\n", i, tnode->expire);
                node = node->next;
            }
        }
    }
}

void print_timerwheel() {
    _print_timerwheel(tw.tvroot.vec, 0, TVR_SIZE);
    _print_timerwheel(tw.tv[0].vec, 1, TVN_SIZE);
    _print_timerwheel(tw.tv[1].vec, 2, TVN_SIZE);
    _print_timerwheel(tw.tv[2].vec, 3, TVN_SIZE);
    _print_timerwheel(tw.tv[3].vec, 4, TVN_SIZE);
}


void on_timer(void *ud) {
    printf("on_timer: %d, data: %d\n", tw.currtick, ((intptr_t)ud));
    timerwheel_del(&tw, &node2);

    timerwheel_node_init(&node3, on_timer, NULL);
    timerwheel_add(&tw, &node3, 1000);
}

void run() {
    int i = 0;
    while (1) {
        ++i;
        usleep(1000);
        timerwheel_update(&tw, gettime());
    }
}

int main(int argc, char const *argv[])
{
    timerwheel_init(&tw, 1, gettime());
    tw.currtick = 0xFFFFFFFF;

    timerwheel_node_init(&node, on_timer, (void*)(intptr_t)1);
    timerwheel_add(&tw, &node, 400);

    timerwheel_node_init(&node2, on_timer, NULL);
    timerwheel_add(&tw, &node2, 400);

    timerwheel_node_init(&node4, on_timer, NULL);
    timerwheel_add(&tw, &node4, 0xF0FFFFFF);

    print_timerwheel();
    run();
    return 0;
}
