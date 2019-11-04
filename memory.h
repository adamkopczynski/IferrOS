#ifndef __MEMORY__
#define __MEMORY__

#include <stddef.h>
#include <stdint.h>

#define PAGE_SIZE 0x1000
#define PAGE_COUNT 0x100000

#define PAGE_PER_STATUS_BYTE 4

#define PAGE_FREE 0
#define PAGE_USED 1
#define PAGE_SYS  2
#define PAGE_NONE 3

#define HEAP_START 0x400000


void memorymap_initialize(void);
void page_set(uint32_t addr, uint32_t state);
void page_set_range(uint32_t addr_start, uint32_t len, uint32_t state);
uint32_t page_get(uint32_t addr);
uint32_t page_claim(uint32_t count);
void debug_display_used_pages(void);

#endif