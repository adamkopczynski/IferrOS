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
#include "../clock.h"
#include "../fat.h"
#include "../fat_shell.h"

//Libc
#include "../libc/stdio.h"

void kernel_main(void)
{
    /* Initialize terminal interface */
    terminal_initialize();
    multiboot_initialize();
    paging_initialize();
    init_heap();
    init_shell();

    // init_ps2();
    init_gdt();
    init_idt();
    init_keyboard();
    init_clock();
    init_threads();

    terminal_clear();
    init_fs();
    
    //Registers commands for shell
    register_heap_stats();
    register_terminal_commands();
    register_fat_shell_api();

    printf("Kernel initialized!\nHello in IferrOS");

    terminal_clear();
    shell_main();
}