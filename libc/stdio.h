#ifndef __STDIO_H__
#define __STDIO_H__

#define EOF (-1)

int printf(char *fmt, ...);
int puts(const char* string);
int putchar(char c);
int gets(char* text, int size);
// int scanf(const char* format, ...);

#endif