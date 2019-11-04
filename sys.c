#include "sys.h"
#include "terminal.h"
#include "stdlib/stdio.h"

void kernel_panic(const char *message)
{
    asm("cli");

    // Czerwony napis Kernel panic w ramce
    terminal_setcolor(VGA_COLOR_RED);
    printf("\n\n\n\n\n\n");
    display_center("\xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB");
    display_center("\xBA Kernel Panic \xBA");
    display_center("\xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC");

    // Komunikat
    terminal_setcolor(VGA_COLOR_WHITE);
    printf("\n\n");
    display_center("We are sorry, but a critical error occurred");
    display_center("Please restart Your system");
    printf("\n\n\n");

    // Wiadomość
    terminal_setcolor(VGA_COLOR_RED);
    display_center(message);

    // Zatrzymanie
    while(1) asm("hlt");
}

// Wyświetla błąd który wystąpił, ale nie wymaga zatrzymania systemu
void report_error(const char *message)
{
    uint8_t last_color = terminal_getcolor();
    terminal_setcolor(VGA_COLOR_RED);
    printf("Error: %s\n", message);
    terminal_setcolor(last_color);
}