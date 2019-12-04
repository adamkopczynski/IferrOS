#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "kernel.h"
#include "../multiboot.h"
#include "../terminal.h"
#include "../gdt.h"
#include "../paging.h"
#include "../heap.h"

//Libc
#include "../libc/stdio.h"

void kernel_main(void)
{
    /* Initialize terminal interface */
    terminal_initialize();
    multiboot_initialize();
    init_gdt();
    paging_initialize();
    initialize_heap();

    printf("Kernel initialized!\nHello in IferrOS");
}