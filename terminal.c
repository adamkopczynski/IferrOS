#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "terminal.h"
#include "shell.h"
#include "ports.h"
#include "libc/string.h"
#include "libc/stdlib.h"


//Terminal static methods
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);
static inline uint16_t vga_entry(unsigned char uc, uint8_t color);
static void move_cursor(uint8_t xpos, uint8_t ypos);
static void terminal_scroll();


//Terminal variables
size_t terminal_row = 0;
size_t terminal_column = 0;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*) TERMINAL_BUFFER_ADDRESS;

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

static void move_cursor(uint8_t xpos, uint8_t ypos){

    uint16_t location = ypos * VGA_WIDTH + xpos;

    outb(VGA_COMMAND_PORT, VGA_CURSOR_HIGH); // Set the high cursor byte
    outb(VGA_DATA_PORT, location >> 8);
    outb(VGA_COMMAND_PORT, VGA_CURSOR_LOW); // Set the low cursor byte
    outb(VGA_DATA_PORT, location);
}

static void terminal_scroll(){

    uint16_t blank = vga_entry(' ', terminal_color);

    //Move the lines up
    for(unsigned int i = VGA_WIDTH; i < VGA_WIDTH * VGA_HEIGHT; i++){
		terminal_buffer[i] = terminal_buffer[i+VGA_WIDTH];
    }

    //Clear the last line
    for(unsigned int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++){
        terminal_buffer[i] = blank;
        terminal_column = 0;
    }

	terminal_row--;
}

void terminal_initialize(void){

	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);

	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

uint8_t terminal_getcolor(void) {
	return terminal_color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
	move_cursor(x+1, y);
}

void terminal_putchar(char c) {

    if(c == '\n'){
        terminal_column=0;
        terminal_row++;
        return;
    }

	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		terminal_row++;	
	}

	if(terminal_row >= VGA_HEIGHT){
		terminal_scroll();
	}

}
 
void terminal_write(const char* data, size_t size) {

	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
int terminal_writestring(const char* data) {

	terminal_write(data, strlen(data));
	return strlen(data);
}

void terminal_clear(void){

    terminal_row = 0;
	terminal_column = 0;
                                           
	for (uint32_t y = 0; y < VGA_HEIGHT; y++) {
		for (uint32_t x = 0; x < VGA_WIDTH; x++) {
			uint32_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}

}


void terminal_workspace_clear(void){

	for(uint32_t i=0; i<VGA_HEIGHT; i++)
		terminal_putchar('\n');

}

int terminal_write_dec(uint32_t d){

    char buff[13];
    char *x = itoa(d, buff, 13);
    terminal_writestring(x);

	return strlen(x);
}

int terminal_write_hex(uint32_t d){

    terminal_writestring("0x");
    for(int i = 28; i >= 0; i-=4)
    {
        terminal_putchar(hex_char(d>>i));
    }

	return 10;
}

void command_clear(const char* argv, uint32_t argc){
	terminal_clear();
}

void terminal_remove_last_char(void){
	terminal_putentryat(' ', terminal_color, --terminal_column, --terminal_row);
}

void register_terminal_commands(void){
	register_shell_command("clear", "Clear terminal", command_clear);
}