#ifndef __HEAP_H__
#define __HEAP_H__

#include <stdint.h>
#include <stddef.h>

#define HEAP_SIZE sizeof(BLOCK_HEADER_T)

enum block_state { FREE, USED };

typedef struct block_header_t{
    uint32_t size;
    enum block_state state;
    struct block_header_t *next_block;
    struct block_header_t *prev_block;
} BLOCK_HEADER_T;

struct m_list{
    BLOCK_HEADER_T *head;
    BLOCK_HEADER_T *tail;
    uint32_t size;
};

void initialize_heap(void);

void *kmalloc(uint32_t size);
void *krealloc(void *p, uint32_t size);

/**
 * Allocates a contiguous region of memory 'size' in size.
 * If page_align==1, it creates that block starting on a page boundary.
 */
void *kmalloc_ap(uint32_t size, uint8_t page_align, uint32_t *phys);

//Free memory pointed by p
void kfree(void *p);

//Show heap stats on terminal
void heap_stats(void);

#endif