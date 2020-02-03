#include <stddef.h>
#include <stdint.h>
#include "paging.h"
#include "multiboot.h"
#include "sys.h"
#include "libc/stdio.h"

// Helpers
static void page_set_with_index(uint32_t index, uint32_t state);
static uint32_t page_get_at_index(uint32_t index);

uint8_t memory_map[PAGE_COUNT/PAGE_PER_STATUS_BYTE];

void paging_initialize(void){

    printf("Memory Map Initialization\n");

    // Set all memory free
    page_set_range(0, 0xFFFFFFFF, PAGE_FREE);

    // Reserve first 4Mb for kernel
    page_set_range(0, HEAP_START, PAGE_SYS);

    //Fill memory map with multiboot info
    struct multiboot_mmap_entry *entry = (struct multiboot_mmap_entry *)multiboot_get_struct()->mmap_addr;
    uint32_t count = multiboot_get_struct()->mmap_length / sizeof(struct multiboot_mmap_entry);
    
    for(uint32_t i=0; i<count; i++){

        if(entry->type != MULTIBOOT_MEMORY_AVAILABLE) 
            page_set_range((uint32_t)entry->addr, entry->len, PAGE_NONE);
        
        entry++;
    }
}

// Set page's state with address
void page_set(uint32_t addr, uint32_t state){

    uint32_t page_nr = addr / PAGE_SIZE;
    page_set_with_index(page_nr, state);
}

// Get page's state with address
uint32_t page_get(uint32_t addr){
    
    uint32_t page_nr = addr / PAGE_SIZE;
    uint32_t state = page_get_at_index(page_nr);
    return state;
}

// Ustala stan wszystkim stronom w podanym zakresie adresów, włączając w to krańce
void page_set_range(uint32_t addr_start, uint32_t len, uint32_t state)
{
    uint32_t addr_end = addr_start + len;

    uint32_t first_page_nr = addr_start / PAGE_SIZE;
    uint32_t last_page_nr = addr_end / PAGE_SIZE;

    for(; first_page_nr < last_page_nr; first_page_nr++)
        page_set_with_index(first_page_nr, state);
}

// Get count pages and set their state as USED
uint32_t page_claim(uint32_t count){

    for(uint32_t i=0; i <= PAGE_COUNT-count; i++){

        int found = 1; 

        for(uint32_t j=0; j<count; j++){

            uint32_t state = page_get_at_index(i+j);
            if(state != PAGE_FREE){

                found = 0; //If there's no enough free pages set to 0
                break;
            }
        }

        if(found){

            for(uint32_t j=0; j<count; j++)
                page_set_with_index(i+j, PAGE_USED);

            return i*PAGE_SIZE;
        }
    }

    kernel_panic("Unable To Find Free Pages");
    return 0;
}

// Display all USED pages
void pages_stats(void)
{
    printf("\nUsed pages: ");
    for(uint32_t i=0; i<PAGE_COUNT; i++){

        if(page_get_at_index(i)==PAGE_USED)
            printf("%d ", i);
    }

    printf("\n");
}

// Set page with index
static void page_set_with_index(uint32_t index, uint32_t state){

    if(index >= PAGE_COUNT) 
        kernel_panic("Invalid page number");

    uint32_t byte = index / PAGE_PER_STATUS_BYTE;
    uint32_t part = index % PAGE_PER_STATUS_BYTE;

    uint8_t *page_byte = memory_map + byte;

    uint8_t mask = 0b11000000 >> part*2;
    *page_byte &= ~mask;

    uint8_t new_state = (uint8_t)state;
    new_state <<= (3-part)*2;
    *page_byte |= new_state;
}

// Get page's state at index
static uint32_t page_get_at_index(uint32_t index){

    if(index >= PAGE_COUNT) 
        kernel_panic("Invalid page number");

    uint32_t byte = index / PAGE_PER_STATUS_BYTE;
    uint32_t part = index % PAGE_PER_STATUS_BYTE;

    uint8_t *page_byte = memory_map + byte;

    uint8_t mask = 0b11000000 >> part*2;
    uint8_t result = *page_byte & mask;
    result >>= (3-part)*2;

    return result;
}