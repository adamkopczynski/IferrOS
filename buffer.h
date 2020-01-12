#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdint.h>
#define BUFFER_SIZE 256

typedef struct circular_buffer_t {
    char* ptr;
    uint32_t begin;
    uint32_t end;
    uint32_t capacity;
    unsigned char full: 1;
} buffer_t;

int circular_buffer_create(struct circular_buffer_t *b, uint32_t size);
void circular_buffer_destroy(struct circular_buffer_t *b);

uint8_t circular_buffer_push_back(struct circular_buffer_t *b, char c);
char circular_buffer_pop_front(struct circular_buffer_t *b, uint8_t *err_code);
char circular_buffer_pop_back(struct circular_buffer_t *b, uint8_t *err_code);

uint8_t circular_buffer_empty(const struct circular_buffer_t *b);
uint8_t circular_buffer_full(const struct circular_buffer_t *b);

//void circular_buffer_display(const struct circular_buffer_t *a);

#endif