#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "kernel.h"
#include "../terminal.h"

void kernel_main(void)
{
    /* Initialize terminal interface */
    terminal_initialize();

    terminal_writestring("Kernel initialized!\nHello in IferrOS");
}