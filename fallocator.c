#include "fallocator.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stddef.h>

#define MAX_BLOCK_SIZE 16*1024
#define MIN_BLOCK_SIZE 4096
#define MIN_ITEM_SIZE sizeof(blockitem_t)
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(v, mi, ma) MAX(MIN(v, ma), mi)
#define FREE_FLAG 0xC0C0C0C0

void fallocator_init(fallocator_t *alloc, uint32_t blocksize, uint32_t itemsize) {
    memset(alloc, 0, sizeof(*alloc));
    alloc->blocksize = CLAMP(blocksize, MIN_BLOCK_SIZE, MAX_BLOCK_SIZE);
    alloc->itemsize = MAX(MIN_ITEM_SIZE, itemsize); 
}

void fallocator_free(fallocator_t *alloc) {
    memblock_t *block = alloc->memblock;
    while (block) {
        memblock_t *temp = block->next;
        free(block);
        block = temp;
    }
}

void* fallocator_newitem(fallocator_t *alloc) {
    if (!alloc->freeitem) {
        // printf("new block\n");
        memblock_t *block = malloc(alloc->blocksize);
        block->next = alloc->memblock;
        alloc->memblock = block;
        int idx = 0;
        int itemsize = alloc->itemsize;
        int blocksize = alloc->blocksize - offsetof(memblock_t, buffer);
        while (idx + itemsize <= blocksize) {
            blockitem_t *item = (blockitem_t*)(block->buffer + idx);
            item->next = alloc->freeitem;
            item->flag = 0;
            alloc->freeitem = item;
            idx += itemsize;
        }
    }
    blockitem_t *item = alloc->freeitem;
    alloc->freeitem = alloc->freeitem->next;
    return item;
}

bool fallocator_freeitem(fallocator_t *alloc, void *item) {
    blockitem_t *bitem = (blockitem_t *)item;
    if (bitem->flag == FREE_FLAG)
        return false;
    bitem->flag = FREE_FLAG;
    bitem->next = alloc->freeitem;
    alloc->freeitem = bitem;
    return true;
}

bool fallocator_isfree(fallocator_t *alloc, void *item) {
    blockitem_t *bitem = (blockitem_t *)item;
    return bitem->flag == FREE_FLAG;
}