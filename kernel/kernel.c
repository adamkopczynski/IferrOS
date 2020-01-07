#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "kernel.h"
#include "../multiboot.h"
#include "../terminal.h"
#include "../gdt.h"
#include "../paging.h"
#include "../heap.h"
#include "../idt.h"
#include "../pic.h"
#include "../keyboard.h"
#include "../threads.h"
#include "../ps2.h"
#include "../shell.h"
#include "../isr.h"

//Libc
#include "../libc/stdio.h"

void kernel_main(void)
{
    /* Initialize terminal interface */
    terminal_initialize();
    multiboot_initialize();
    init_gdt();

    init_idt();

    // paging_initialize();
    init_heap();
    // init_ps2();
    init_keyboard();
    // init_threads();
    init_shell();

    printf("Kernel initialized!\nHello in IferrOS");

    // heap_stats();

    terminal_clear();

    shell_main();
}