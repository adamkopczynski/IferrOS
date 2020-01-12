#include <stddef.h>
#include <stdint.h>
#include "heap.h"
#include "terminal.h"
#include "sys.h"
#include "paging.h"
#include "multiboot.h"
#include "shell.h"
#include "libc/stdio.h"

static memory_block_t *get_pages(uint32_t size);
static void free_pages(memory_block_t *block);
static memory_block_t *memory_merge(memory_block_t *block);
static memory_block_t *memory_split(memory_block_t *block, uint32_t size);
static memory_block_t *create_block_at(uint32_t address, memory_block_t *prev, memory_block_t *next, uint32_t size, int state);

memory_list_t mlist;

void init_heap(void){

    printf("Heap Initialization\n");
    mlist.head = NULL;
    mlist.tail = NULL;
    mlist.size = 0;
}

void *__kmalloc(uint32_t size, const char *filename, uint32_t line){

    memory_block_t *current = mlist.head;
    memory_block_t *found = NULL;

    for(uint32_t i=0; i<mlist.size; i++){

        //Search for FREE block with required size
        if(current->state == FREE && current->size >= size) {
            found = current;
            break; 
        }

        current = current->next_block;
    }

    //If there's no block, get new pages
    if(found == NULL) 
        found = get_pages(size);

    //Split block if size is bigger than required
    if((int64_t)found->size - (int64_t)size - (int64_t)HSIZE > 0)
        found = memory_split(found, size);

    found->state = USED;
    found->filename = filename;
    found->line = line;

    return found+1;
}

void *__kcalloc(uint32_t size, const char *filename, uint32_t line){

    //Allocate memory
    char *ptr = __kmalloc(size, filename, line);

    if(ptr == NULL) 
        return NULL;

    //Fill it with zeros
    for(uint32_t i=0; i<size; i++)
        *(ptr + i) = 0;

    return ptr;
}

void *__krealloc(void *ptr, uint32_t new_size, const char *filename, uint32_t line){

    //Allocate memory fo NULL ptr
    if(ptr == NULL) 
        return __kmalloc(new_size, filename, line);

    memory_block_t *old_block = (memory_block_t*)ptr;
    old_block--;

    //Current size is enough
    if(old_block->size >= new_size)
        return ptr;

    char *new_ptr = __kmalloc(new_size, filename, line);

    if(new_ptr == NULL) 
        return NULL;

    //Typecast pointer to char type (1 byte size)
    char *old_ptr = (char*)ptr;
    
    for(uint32_t i=0; i < old_block->size; i++)
        *(new_ptr+i) = *(old_ptr + i);
    
    //Free memory for old ptr
    kfree(ptr);

    return new_ptr;
}

void kfree(void *ptr){

    memory_block_t *block = (memory_block_t*)ptr;
    block--;

    if(block->state != USED){

        report_error("Memory is free\n");
        return;
    }

    block->state = FREE;

    //Merge free blocks
    memory_block_t *merged = NULL;

    if(block != mlist.tail && block->next_block->state == FREE) 
        merged = memory_merge(block);

    if(merged) 
        block = merged;

    if(block != mlist.head && block->prev_block->state == FREE) 
        merged = memory_merge(block->prev_block);

    if(merged) 
        block = merged;

    free_pages(block);
}

void heap_stats(void){

    memory_block_t *current = mlist.head;

    // terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
    printf("| BEGIN          | LEN            | TYPE           | FILE           | LINE\n");
    // terminal_setcolor(VGA_COLOR_WHITE);
    for(uint32_t i=0; i<mlist.size; i++)
    {
        int count = printf("| %d", (unsigned int)current);
        for(; count<8; count++) printf(" ");
        count = printf("| %d", current->size);
        for(; count<8; count++) printf(" ");
        if(current->state == FREE) count = printf("| Free");
        if(current->state == USED) count = printf("| Used");
        for(; count<8; count++) printf(" ");
        if(current->state==USED) count = printf("| %s", current->filename);
        else count = printf("| ");
        for(; count<8; count++) printf(" ");
        if(current->state==USED) count = printf("| %d\n", current->line);
        else count = printf("| \n");
        current = current->next_block;
    }
}

static memory_block_t *create_block_at(uint32_t address, memory_block_t *prev, memory_block_t *next, uint32_t size, int state){

    memory_block_t *header = (memory_block_t*)address;
    header->prev_block = prev;
    header->next_block = next;
    header->size = size;
    header->state = state;
    return header;
}

static memory_block_t *memory_split(memory_block_t *block, uint32_t size){

    if(block->size <= HSIZE || block->state != FREE)
        return NULL;

    uint32_t new_header_address = (uint32_t)block + sizeof(memory_block_t) + size;
    memory_block_t *new_block = create_block_at(new_header_address, block, block->next_block, block->size - size - HSIZE, FREE);

    if(block == mlist.tail) 
        mlist.tail = new_block;
    else 
        block->next_block->prev_block = new_block;

    block->next_block = new_block;
    block->size = size;

    mlist.size++;

    return block;
}

static memory_block_t *memory_merge(memory_block_t *block){

    if(block == mlist.tail || block->state != FREE || block->next_block->state != FREE)
        return NULL;

    if((uint32_t)block + block->size + HSIZE != (uint32_t)block->next_block)
        return NULL;

    if(block->next_block == mlist.tail) 
        mlist.tail = block;
    
    mlist.size--;

    block->size = block->size + block->next_block->size + HSIZE;
    block->next_block = block->next_block->next_block;
    block->next_block->prev_block = block;

    return block;
}

static memory_block_t *get_pages(uint32_t size){

        uint32_t pages_count = (size + HSIZE) / PAGE_SIZE;
        pages_count += ((size + HSIZE) % PAGE_SIZE) > 0;

        uint32_t page_addr = page_claim(pages_count);

        memory_block_t *block = create_block_at(page_addr, mlist.tail, NULL, pages_count*PAGE_SIZE-HSIZE, FREE);

        if(mlist.size == 0) 
            mlist.head = block;
        else 
            mlist.tail->next_block = block;

        mlist.tail = block;
        mlist.size++;

        return block;
}

static void free_pages(memory_block_t *block){
    
    uint32_t page_start = (uint32_t)block;

    if((uint32_t)block % PAGE_SIZE != 0) 
        page_start = (uint32_t)block + PAGE_SIZE - (uint32_t)block % PAGE_SIZE;
    
    uint32_t block_end = (uint32_t)block + block->size + HSIZE;
    uint32_t len = block_end - page_start;

    if(len >= PAGE_SIZE && block_end > page_start){

        if((uint32_t)block < page_start) 
            block = memory_split(block, page_start - (uint32_t)block - HSIZE)->next_block;
        
        if(block == mlist.head) 
            mlist.head = block->next_block;
        else 
            block->prev_block->next_block = block->next_block;
        
        if(block == mlist.tail) 
            mlist.tail = block->prev_block;
        else 
            block->next_block->prev_block = block->prev_block;
        
        mlist.size--;
        page_set_range(page_start, len, PAGE_FREE);
    }
}