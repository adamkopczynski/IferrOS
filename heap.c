#include <stdint.h>
#include <stddef.h>
#include "heap.h"
#include "paging.h"
#include "libc/stdio.h"

//static helpers
static BLOCK_HEADER_T *create_new_pages(uint32_t size);
// static void free_empty_pages(BLOCK_HEADER_T *block);
// static BLOCK_HEADER_T *memory_join(BLOCK_HEADER_T *block);
// static BLOCK_HEADER_T *memory_split(BLOCK_HEADER_T *block, uint32_t size);
// static BLOCK_HEADER_T *create_header_at(uint32_t address, BLOCK_HEADER_T *prev, BLOCK_HEADER_T *next, uint32_t size, int state);
// static void heap_command_heapinfo(const char* argv, uint32_t argc);

struct m_list m_list;

void initialize_heap(void){

    printf("Initialize heap");

    //Init empty heap
    m_list.head = NULL;
    m_list.tail = NULL;
    m_list.size = 0;

}

void *kmalloc(uint32_t size){

    BLOCK_HEADER_T *current = m_list.head;
    BLOCK_HEADER_T *mptr = NULL; //pointer to new allocing block

    //Find free block with required size
    for(uint32_t i = 0; i < m_list.size; i++){

        if(current->size == size && current->state == FREE){
            mptr = current;
            break;
        }

        current = current->next_block;
    }

    //If there's no enough space create new page or pages
    if(mptr == NULL){
        mptr = create_new_pages(size);
    }

    //If block size is enough split it
    // if(mptr->size - size - HEAP_SIZE > 0){
    //     mptr = memory_split(mptr, size);
    // }

    //Mark block as used and return address
    mptr->state = USED;

    return mptr;
}

static BLOCK_HEADER_T *create_new_pages(uint32_t size){

    uint32_t pages = (size+HEAP_SIZE)/PAGE_SIZE;
    pages += ((size+HEAP_SIZE)%PAGE_SIZE) > 0;

    uint32_t page_addr = page_claim(pages);

    BLOCK_HEADER_T *header = create_header_at(page_addr, m_list.tail, NULL, (pages*PAGE_SIZE)-HEAP_SIZE, FREE);

    if(m_list.size == 0){
        m_list.head = header;
    }
    else
        m_list.tail->next_block = header;

    m.tail = header;
    m.size++;

    return header;
}
