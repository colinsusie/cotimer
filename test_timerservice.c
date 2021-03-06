#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include "timerservice.h"

timerservice_t *sv;
void *thandle;
int count;

static uint64_t gettime() {
    struct timeval tm;
    gettimeofday(&tm, NULL);
    return (tm.tv_sec * 1000 + tm.tv_usec / 1000);
}

void on_timer(void *ud) {
    printf("on_timer: %llu\n", gettime());
    ++count;
    if (count == 10)
        ts_del_timer(sv, &thandle);
}

void run() {
    while (1) {
        usleep(1000);
        ts_update(sv, gettime());
    }
}


int main(int argc, char const *argv[])
{
    sv = ts_init(10, gettime());

    thandle = ts_add_timer(sv, 100, 100, on_timer, NULL);

    run();

    ts_del_timer(sv, &thandle);
    ts_free(sv);
    return 0;
}
