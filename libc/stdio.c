#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include "stdio.h"
#include "../terminal.h"
#include "../buffer.h"
#include "../keyboard.h"
#include "string.h"

int putchar(char c) {

	terminal_putchar(c);

	return c;
}

int puts(const char* string) {
	return printf("%s\n", string);
}


static bool print(const char* data, size_t length) {
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == EOF)
			return false;
	return true;
}

int printf(char *fmt, ...) {
    //return;
    va_list argp;
    va_start(argp, fmt);

    char *p;
    for(p = fmt; *p != 0; p++) {
        if(*p != '%') {
            terminal_putchar(*p);
            continue;
        }
        p++; // Skip the %
        int i;
        char *s;
        switch(*p) {
        case 'c':
            i = va_arg(argp, int);
            terminal_putchar(i);
            break;
        case 's':
            s = va_arg(argp, char *);
            terminal_writestring(s);
            break;
        case 'd':
            i = va_arg(argp, int);
            terminal_write_dec(i);
            break;
        case 'x':
            i = va_arg(argp, int);
            terminal_write_hex(i);
            break;
        case '%':
            terminal_putchar('%');
            break;
        default:
            terminal_putchar('%');
            terminal_putchar(*p);
            break;
        }
    }
    return 0;
}

int gets(char* text, int size){

	buffer_t* kbuff = keyboard_get_buffer();
	int len = 0;

	while(1){

		if(!circular_buffer_empty(kbuff)){
   
			char c = circular_buffer_pop_back(kbuff, NULL);

			if(c == '\n'){
				putchar('\n');
				*(text+len++) = '\0';
				return len;
			}
			else if(c == '\b' && len > 0){
				//TODO remove last displayed character
				len--;
			}
			else{
				putchar(c);
				*(text+len++) = c;
			}

		}
	}
}
