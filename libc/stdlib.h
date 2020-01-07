#ifndef _STDLIB_H
#define _STDLIB_H

#include <stdint.h>

__attribute__((__noreturn__))
void abort(void);

char* itoa(uint32_t num, char buffer[], int bufflen);
uint8_t hex_char(uint8_t byte);

#endif
