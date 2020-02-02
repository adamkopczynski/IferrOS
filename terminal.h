#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ESC (0x1B)
#define BS  (0x08)
#define EOT (0x04)

#define VGA_COMMAND_PORT 0x3D4
#define VGA_DATA_PORT    0x3D5
#define VGA_CURSOR_HIGH  14
#define VGA_CURSOR_LOW   15

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define TERMINAL_BUFFER_ADDRESS 0xB8000

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

void terminal_initialize(void);
void terminal_setcolor(uint8_t color);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
uint8_t terminal_getcolor(void);
void terminal_putchar(char c);
void terminal_clear(void);
void terminal_workspace_clear(void);
void terminal_write_dec(uint32_t d);
void terminal_write_hex(uint32_t d);
void register_terminal_commands(void);
void terminal_remove_last_char(void);

#endif