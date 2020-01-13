#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdint.h>
#define BUFFER_SIZE 256

typedef struct circular_buffer_t {
    char ptr[BUFFER_SIZE];
    int begin;
    int end;
    int capacity;
    unsigned char full: 1;
} buffer_t;

int circular_buffer_create(struct circular_buffer_t *b, int size);
void circular_buffer_destroy(struct circular_buffer_t *b);

int circular_buffer_push_back(struct circular_buffer_t *b, char c);
char circular_buffer_pop_front(struct circular_buffer_t *b, int *err_code);
char circular_buffer_pop_back(struct circular_buffer_t *b, int *err_code);

int circular_buffer_empty(const struct circular_buffer_t *b);
int circular_buffer_full(const struct circular_buffer_t *b);

//void circular_buffer_display(const struct circular_buffer_t *a);

#endif