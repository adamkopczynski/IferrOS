#include <stdint.h>
#include "clock.h"
#include "isr.h"
#include "shell.h"
#include "libc/stdio.h"

static void set_frequency(unsigned int frequency);
static void clock_command_timesys(const char* argv, uint32_t argc);

volatile unsigned long long ticks = 0;
volatile double system_time_ms = 0;
double IRQ0_interval_ms = 0;
double  IRQ0_frequency = 0;

void init_clock(void){
    set_frequency(FREQUENCY);
    register_interrupt_handler(IRQ0, clock_interrupt_handler);
    register_shell_command("systime", "System clock ticks", clock_command_timesys);
}

static void set_frequency(unsigned int frequency){

    unsigned short divider = (unsigned short)(1193182.0 / frequency);

    IRQ0_frequency = 1193182.0 / divider;
    IRQ0_interval_ms = 1000.0/IRQ0_frequency;

    set_PIT_count(divider);
}

void sleep(unsigned long long time){

    unsigned long long end = ticks + time;
    while(ticks < end) continue;
}

unsigned long long clock(void){

    return ticks;
}

double time(void){

    return system_time_ms;
}

void clock_interrupt_handler(void){

    system_time_ms += IRQ0_interval_ms;
    ticks += 1;
}

static void clock_command_timesys(const char* tokens, uint32_t tokens_count){

    terminal_setcolor(VGA_COLOR_WHITE);
    printf("Time in ms since start: ");
    terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
    printf("%d\n", ticks);
}