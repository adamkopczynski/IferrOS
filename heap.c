// #include <stdint.h>
// #include <stddef.h>
// #include "heap.h"
// #include "paging.h"
// #include "sys.c"
// #include "libc/stdio.h"

// //static helpers
// static BLOCK_HEADER_T *create_new_pages(uint32_t size);
// static void free_empty_pages(BLOCK_HEADER_T *block);
// static BLOCK_HEADER_T *blocks_join(BLOCK_HEADER_T *block);
// static BLOCK_HEADER_T *block_split(BLOCK_HEADER_T *block, uint32_t size);
// static BLOCK_HEADER_T *create_header_at(uint32_t address, BLOCK_HEADER_T *prev, BLOCK_HEADER_T *next, uint32_t size, int state);
// // static void heap_command_heapinfo(const char* argv, uint32_t argc);

// struct m_list m_list;

// void initialize_heap(void){

//     printf("Initialize heap");

//     //Init empty heap
//     m_list.head = NULL;
//     m_list.tail = NULL;
//     m_list.size = 0;

// }

// void *kmalloc(uint32_t size){

//     BLOCK_HEADER_T *current = m_list.head;
//     BLOCK_HEADER_T *mptr = NULL; //pointer to new allocing block

//     //Find free block with required size
//     for(uint32_t i = 0; i < m_list.size; i++){

//         if(current->size == size && current->state == FREE){
//             mptr = current;
//             break;
//         }

//         current = current->next_block;
//     }

//     //If there's no enough space create new page or pages
//     if(mptr == NULL){
//         mptr = create_new_pages(size);
//     }

//     //If block size is enough split it
//     if(mptr->size - size - HEAP_SIZE > 0){
//         mptr = block_split(mptr, size);
//     }

//     // Mark block as used and return address
//     mptr->state = USED;

//     return mptr;
// }

// static BLOCK_HEADER_T *create_new_pages(uint32_t size){

//     uint32_t pages = (size+HEAP_SIZE)/PAGE_SIZE;
//     pages += ((size+HEAP_SIZE)%PAGE_SIZE) > 0;

//     uint32_t page_addr = page_claim(pages);

//     BLOCK_HEADER_T *header = create_header_at(page_addr, m_list.tail, NULL, (pages*PAGE_SIZE)-HEAP_SIZE, FREE);

//     if(m_list.size == 0){
//         m_list.head = header;
//     }
//     else
//         m_list.tail->next_block = header;

//     m_list.tail = header;
//     m_list.size++;

//     return header;
// }

// static BLOCK_HEADER_T *create_header_at(uint32_t address, BLOCK_HEADER_T *prev, BLOCK_HEADER_T *next, uint32_t size, int state){
   
//     BLOCK_HEADER_T *header = (BLOCK_HEADER_T*)address;

//     header->prev_block = prev;
//     header->next_block = next;
//     header->size = size;
//     header->state = state;

//     return header;
// }

// static BLOCK_HEADER_T *block_split(BLOCK_HEADER_T *block, uint32_t size){

//     if(block->size <= HEAP_SIZE || block->state != FREE)
//         return NULL;

//     uint32_t new_header_address = (uint32_t)block + sizeof(BLOCK_HEADER_T) + size;
//     BLOCK_HEADER_T *new_block = create_header_at(new_header_address, block, block->next_block, block->size - size - HEAP_SIZE, FREE);

//     if(block == m_list.tail) m_list.tail = new_block;
//     else block->next_block->prev_block = new_block;

//     block->next_block = new_block;
//     block->size = size;

//     m_list.size++;

//     return block;
// }

// static BLOCK_HEADER_T *blocks_join(BLOCK_HEADER_T *block){

//     if(block == m_list.tail || block->state != FREE || block->next_block->state != FREE) return NULL;

//     // Sąsiednie bloki z listy mogą pochodzić z różnich obszarów pamięci więc trzeba sprawdzić czy na prawde sąsiadują
//     if((uint32_t)block + block->size + HEAP_SIZE != (uint32_t)block->next_block) return NULL;

//     if(block->next_block == m_list.tail) m_list.tail = block;
//     m_list.size--;

//     block->size = block->size + block->next_block->size + HEAP_SIZE;
//     block->next_block = block->next_block->next_block;
//     block->next_block->prev_block = block;

//     return block;
// }

// static void free_empty_pages(BLOCK_HEADER_T *block){
    
//     uint32_t page_start = (uint32_t)block;
//     if((uint32_t)block % PAGE_SIZE != 0) page_start = (uint32_t)block + PAGE_SIZE - (uint32_t)block % PAGE_SIZE;
//     uint32_t block_end = (uint32_t)block + block->size + HEAP_SIZE;
//     uint32_t len = block_end - page_start;

//     if(len >= PAGE_SIZE && block_end > page_start) {

//         if((uint32_t)block < page_start) block = block_split(block, page_start - (uint32_t)block - HEAP_SIZE)->next_block;
//         if(block == m_list.head) m_list.head = block->next_block;
//         else block->prev_block->next_block = block->next_block;
//         if(block == m_list.tail) m_list.tail = block->prev_block;
//         else block->next_block->prev_block = block->prev_block;
//         m_list.size--;

//         page_set_range(page_start, len, PAGE_FREE);

//     }
// }