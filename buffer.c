#include <stddef.h>
#include <stdint.h>
#include "heap.h"
#include "libc/stdio.h"
#include "buffer.h"

int circular_buffer_empty(const struct circular_buffer_t *b){

    if(!b || !b->ptr || b->begin < 0 ||
    b->end < 0 || b->capacity <= 0 || b->begin >= b->capacity || b->end >= b->capacity
  ) return -1;

    if(!b->full && b->end == b->begin) return 1;
    
    return 0;
}

int circular_buffer_full(const struct circular_buffer_t *b){

  if(!b || !b->ptr || b->begin < 0 ||
    b->end < 0 || b->capacity <= 0 || b->begin >= b->capacity || b->end >= b->capacity) return -1;

    return b->full;
}

int circular_buffer_create(struct circular_buffer_t *b, int size){

  b->end = 0;
  b->begin = 0;
  b->capacity = BUFFER_SIZE;
  b->full = 0;

  return 0;
}

void circular_buffer_destroy(struct circular_buffer_t *b){
  
  if(b){
    kfree(b->ptr);
  }
}

int circular_buffer_push_back(struct circular_buffer_t *cb, char value){

  if(!cb || !cb->ptr || cb->begin < 0 ||
    cb->end < 0 || cb->capacity <= 0 || cb->begin >= cb->capacity || cb->end >= cb->capacity
  ) return 1;

  if(!cb->full){
    *((cb->ptr)+(cb->end)) = value;
  }
  else{
    *((cb->ptr)+(cb->begin)) = value;
    cb->begin = (cb->begin+1)%cb->capacity;
  }

  cb->end = (cb->end+1)%cb->capacity;
  cb->full = cb->end == cb->begin ? 1 : 0;

  return 0;
}

char circular_buffer_pop_front(struct circular_buffer_t *a, int *err_code){

  char poped_value;

  if(!a || !a->ptr || a->begin < 0 ||
    a->end < 0 || a->capacity <= 0 || a->begin >= a->capacity || a->end >= a->capacity) {
    if(err_code) *err_code = 1;
    return 0;
  }

  if(circular_buffer_empty(a)){
    if(err_code) *err_code = 2;
    return 0;
  }
  else{
    poped_value = *(a->ptr+(a->begin));
    a->begin = (a->begin+1)%a->capacity;

    if(circular_buffer_full(a))
      a->full = 0;
  }

  if(err_code) *err_code = 0;
  return poped_value;
}

char circular_buffer_pop_back(struct circular_buffer_t *a, int *err_code){

  char poped_value;

  if(!a || !a->ptr || a->begin < 0 ||
    a->end < 0 || a->capacity <= 0 || a->begin >= a->capacity || a->end >= a->capacity) {
    if(err_code) *err_code = 1;
    return 1;
  }
  
  if(circular_buffer_empty(a)){
    if(err_code) *err_code = 2;
    return 2;
  }
  else{
    a->end = (a->end+a->capacity-1)%a->capacity;
    poped_value = *(a->ptr+(a->end));

    if(circular_buffer_full(a))
      a->full = 0;
  }

  if(err_code) *err_code = 0;
  return poped_value;
}

// void circular_buffer_display(const struct circular_buffer_t *a){

//   if(a && a->ptr && a->begin >= 0 &&
//     a->end >= 0 && a->capacity > 0 && a->end < a->capacity && a->begin < a->capacity && !circular_buffer_empty(a)){

//     if(a->end <= a->begin){
//       for(int i = a->begin; i < a->capacity; i++){
//         printf("%d ", *(a->ptr+i));
//       }
//       for(int i = 0; i < a->end; i++){
//         printf("%d ", *(a->ptr+i));
//       }
//     }
//     else{
//       for(int i = a->begin; i < a->end; i++){
//         printf("%d ", *(a->ptr+i));
//       }
//     }
//     printf("\n");
//   }
// }