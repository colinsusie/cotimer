#pragma once
#include "fallocator.h"
#include "timerwheel.h"

// 定时器服务
typedef struct timerservice {
    fallocator_t alloc;
    timerwheel_t twheel;
} timerservice_t;

// 初始化定时器服务
timerservice_t* ts_init(uint16_t interval, uint64_t currtime);
// 释放定时器服务
void ts_free(timerservice_t* sv);
// 增加定时器
void* ts_add_timer(timerservice_t *sv, 
    uint32_t delay,     // 首次延迟
    uint32_t loop,      // 后续的间隔，如果为0就没有后续
    timer_cb_t cb,      // 回调函数
    void *ud);          // 用户数据
// 删除定时器
void ts_del_timer(timerservice_t *sv, void **handle);
// 更新定时器
void ts_update(timerservice_t *sv, uint64_t currtime);