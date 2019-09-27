#pragma once
/**
 * 固定长度的内存分配器
*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct blockitem {
    struct blockitem *next;
    uint32_t flag;
} blockitem_t;

typedef struct memblock {
    struct memblock *next;
    char buffer[0];
} memblock_t;

typedef struct fallocator {
    memblock_t *memblock;
    uint32_t blocksize;
    uint32_t itemsize;
    blockitem_t *freeitem;
} fallocator_t;

// 初始化分配器: blocksize为内存块的长度，itemsize为分配项的大小
void fallocator_init(fallocator_t *alloc, uint32_t blocksize, uint32_t itemsize);
// 释放分配器
void fallocator_free(fallocator_t *alloc);
// 创建内存项
void* fallocator_newitem(fallocator_t *alloc);
// 释放内存器
bool fallocator_freeitem(fallocator_t *alloc, void *item);
// 判断内存项是否已经释放 
bool fallocator_isfree(fallocator_t *alloc, void *item);


