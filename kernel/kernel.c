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

//Libc
#include "../libc/stdio.h"

void kernel_main(void)
{
    /* Initialize terminal interface */
    terminal_initialize();
    multiboot_initialize();
    init_gdt();

    PIC_remap(0x20, 0x28);
    init_idt();

    paging_initialize();
    // initialize_heap();
    init_keyboard();

    printf("Kernel initialized!\nHello in IferrOS");
}