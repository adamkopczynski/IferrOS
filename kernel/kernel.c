#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "kernel.h"
#include "../terminal.h"
#include "../gdt.h";

//Stdlib
#include "../stdlib/stdio.h";

void kernel_main(void)
{
    /* Initialize terminal interface */
    terminal_initialize();
    gdt_initialize();

    printf("Kernel initialized!\nHello in IferrOS");
}