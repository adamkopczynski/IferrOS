#include "stdlib.h"
#include "stdio.h"

__attribute__((__noreturn__))
void abort(void) {
#if defined(__is_libk)
	// TODO: Add proper kernel panic.
	printf("kernel: panic: abort()\n");
#else
	// TODO: Abnormally terminate the process as if by SIGABRT.
	printf("abort()\n");
#endif
	while (1) { }
	__builtin_unreachable();
}


char * itoa(uint32_t num, char buffer[], int bufflen){
    int i = bufflen - 2;
    buffer[bufflen-1] = 0;

    if(num == 0) {
        buffer[i--] = '0';
    }

    while(num > 0 && i >= 0)
    {
        buffer[i--] = (num % 10) + '0';
        num/=10;
    }

    return &buffer[i+1];
}

uint8_t hex_char(uint8_t byte){

    byte = byte & 0x0F;
    if(byte < 0xA){

        char buff[2];
        itoa(byte, buff, 2);
        return buff[0];
    }
    else{

        switch(byte){

        case 0x0A:
            return 'A';
            break;
        case 0x0B:
            return 'B';
            break;
        case 0x0C:
            return 'C';
            break;
        case 0x0D:
            return 'D';
            break;
        case 0x0E:
            return 'E';
            break;
        case 0x0F:
            return 'F';
            break;
        }
    }
    return 0;
}
