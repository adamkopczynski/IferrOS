#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

size_t strlen(const char* str);
int memcmp(const void* aptr, const void* bptr, size_t size);
void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size);
void* memmove(void* dstptr, const void* srcptr, size_t size);
void* memset(void* bufptr, int value, size_t size);
int strcmp(const char* str, const char* str2);
char to_upper(char c);
#endif