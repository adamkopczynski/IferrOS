#include <stdint.h>
#include "paging.h"
#include "sys.h"
#include "libc/stdio.h"
#include "multiboot.h"

//Static helpers
static void set_page_at_index(uint32_t index, uint32_t state);
static uint32_t get_page_at_index(uint32_t index);

//4GB mmap
uint8_t mmap[PAGE_COUNT/PAGE_PER_STATUS_BYTE];

void paging_initialize(void){

    printf("[+] Paging Initialization\n");

    // Free all memory
    page_set_range(0, 0xFFFFFFFF, PAGE_FREE);

    // Lock first 4Mb for kernel and multiboot
    page_set_range(0, HEAP_START, PAGE_SYS);

    // Fill rest with multiboot data
    struct multiboot_mmap_entry *entry = (struct multiboot_mmap_entry *)multiboot_get_struct()->mmap_addr;
    uint32_t count = multiboot_get_struct()->mmap_length / sizeof(struct multiboot_mmap_entry);
    
    for(uint32_t i=0; i<count; i++)
    {
        if(entry->type != MULTIBOOT_MEMORY_AVAILABLE) page_set_range((uint32_t)entry->addr, entry->len, PAGE_NONE);
        entry++;
    }
}

void page_set(uint32_t addr, uint32_t state){

    uint32_t index = addr/PAGE_SIZE;

    set_page_at_index(index, state);
}

void page_set_range(uint32_t addr_start, uint32_t len, uint32_t state){

    uint32_t addr_end = addr_start + len;

    uint32_t first_page_nr = addr_start / PAGE_SIZE;
    uint32_t last_page_nr = addr_end / PAGE_SIZE;

    for(; first_page_nr < last_page_nr; first_page_nr++)
        set_page_at_index(first_page_nr, state);

}

uint32_t page_get(uint32_t addr){

    uint32_t index = addr / PAGE_SIZE;

    uint32_t state = get_page_at_index(index);

    return state;
}

uint32_t page_claim(uint32_t count){

    for(uint32_t i=0; i<=PAGE_COUNT-count; i++)
    {
        int free = 1;
        for(uint32_t j=0; j<count; j++)
        {
            uint32_t state = get_page_at_index(i+j);

            if(state != PAGE_FREE)
            {
                free = 0;
                break;
            }
        }

        if(free){

            for(uint32_t j = 0; j < count; j++)
                set_page_at_index(i+j, PAGE_USED);

            return i*PAGE_SIZE;
        }
    }

    kernel_panic("Unable To Find Free Pages");
    return 0;
}

static void set_page_at_index(uint32_t index, uint32_t state){

    if(index >= PAGE_COUNT) kernel_panic("Invalid Page Number");

    uint32_t byte = index / PAGE_PER_STATUS_BYTE;
    uint32_t part = index % PAGE_PER_STATUS_BYTE;

    uint8_t *page_byte = mmap + byte;

    uint8_t mask = 0b11000000 >> part*2;
    *page_byte &= ~mask;

    uint8_t new_state = (uint8_t)state;
    new_state <<= (3-part)*2;
    *page_byte |= new_state;
}

static uint32_t get_page_at_index(uint32_t index){

    if(index >= PAGE_COUNT) kernel_panic("Invalid Page Number");

    uint32_t byte = index / PAGE_PER_STATUS_BYTE;
    uint32_t part = index % PAGE_PER_STATUS_BYTE;

    uint8_t *page_byte = mmap + byte;

    uint8_t mask = 0b11000000 >> part*2;
    uint8_t result = *page_byte & mask;
    result >>= (3-part)*2;

    return result;
}