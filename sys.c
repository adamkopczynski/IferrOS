#include "sys.h"
#include "terminal.h"
#include "libc/stdio.h"

// Kernel critical error
void kernel_panic(const char *message)
{
    asm("cli");

    // Change color to red
    terminal_setcolor(VGA_COLOR_RED);
    printf("Kernel Panic!");

    // Change color to white
    terminal_setcolor(VGA_COLOR_WHITE);
    printf("\n\n");
    printf("We are sorry, but a critical error occurred");
    printf("Please restart Your system");
    printf("\n\n");

    // Message
    terminal_setcolor(VGA_COLOR_RED);
    printf(message);

    // Stop system
    while(1) asm("hlt");
}

// Report non critical error
void report_error(const char *message)
{
    uint8_t last_color = terminal_getcolor();

    //Change color to red
    terminal_setcolor(VGA_COLOR_RED);
    printf("Error: %s\n", message);

    //Reset color to previous one
    terminal_setcolor(last_color);
}