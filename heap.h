#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include <stddef.h>
#include <stdint.h>

#define HSIZE sizeof(memory_block_t)

enum mem_state { FREE, USED };

typedef struct memory_block_header_t{

    uint32_t size;
    enum mem_state state;
    struct memory_block_header_t *next_block;
    struct memory_block_header_t *prev_block;

    // Dodatkowe pola, ułatwiające debugowanie
    const char *filename;
    uint32_t line;
} memory_block_t;

typedef struct mem_list{

    memory_block_t *head;
    memory_block_t *tail;
    uint32_t size;
} memory_list_t;

// Makra
#define kmalloc(_size) __kmalloc(_size, __FILE__, __LINE__);
#define kcalloc(_size) __kcalloc(_size, __FILE__, __LINE__);
#define krealloc(_ptr, _size) __krealloc(_ptr, _size, __FILE__, __LINE__);

// Prototypy
void init_heap(void);
void heap_full_initialize(void);

void *__kmalloc(uint32_t size, const char *filename, uint32_t line);
void *__kcalloc(uint32_t size, const char *filename, uint32_t line);
void *__krealloc(void *old_ptr, uint32_t new_size, const char *filename, uint32_t line);

void kfree(void *ptr);
void register_heap_stats(void);

#endif