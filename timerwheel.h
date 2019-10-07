#pragma once
/**
 * 定时器模块
 */
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "clinklist.h"

////////////////////////////////////////////////////////////////////////////////////////
// 时间轮定时器

// 第1个轮占的位数
#define TVR_BITS 8
// 第1个轮的长度
#define TVR_SIZE (1 << TVR_BITS)
// 第n个轮占的位数
#define TVN_BITS 6
// 第n个轮的长度
#define TVN_SIZE (1 << TVN_BITS)
// 掩码：取模或整除用
#define TVR_MASK (TVR_SIZE - 1)
#define TVN_MASK (TVN_SIZE - 1)

// 定时器回调函数
typedef void (*timer_cb_t)(void*);

// 定时器结点
typedef struct timernode {
    clinknode_t *next;        // 下一个结点
    clinknode_t *prev;        // 上一个结点
    void *userdata;               // 用户数据
    timer_cb_t callback;          // 回调函数
    uint32_t expire;              // 到期时间
} timernode_t;

// 第1个轮
typedef struct tvroot {
    clinknode_t vec[TVR_SIZE];
} tvroot_t;

// 后面几个轮
typedef struct tvnum {
    clinknode_t vec[TVN_SIZE];
} tvnum_t;



// 时间轮定时器
typedef struct timerwheel {
    tvroot_t tvroot;               // 第1个轮
    tvnum_t tv[4];                 // 后面4个轮
    uint64_t lasttime;             // 上一次的时间毫秒
    uint32_t currtick;             // 当前的tick
    uint16_t interval;             // 每个时间点的毫秒间隔
    uint16_t remainder;            // 剩余的毫秒
} timerwheel_t;


// 初始化时间轮，interval为每帧的间隔，currtime为当前时间
void timerwheel_init(timerwheel_t *tw, uint16_t interval, uint64_t currtime);
// 初始化时间结点：cb为回调，ud为用户数据
void timerwheel_node_init(timernode_t *node, timer_cb_t cb, void *ud);
// 增加时间结点，ticks为触发间隔(注意是以interval为单位)
void timerwheel_add(timerwheel_t *tw, timernode_t *node, uint32_t ticks);
// 删除结点
int timerwheel_del(timerwheel_t *tw, timernode_t *node);
// 更新时间轮
void timerwheel_update(timerwheel_t *tw, uint64_t currtime);
