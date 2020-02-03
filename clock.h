#ifndef __CLOCK_H__
#define __CLOCK_H__
#include <stdint.h>

#define FREQUENCY 1000

extern unsigned short read_PIT_count(void);
extern void set_PIT_count(unsigned short count);

void init_clock(void);
void clock_interrupt_handler(void);
void sleep(unsigned long long time);
unsigned long long clock(void);
double time(void);

#endif