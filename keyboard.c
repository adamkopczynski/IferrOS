#include "keyboard.h"
#include "isr.h"
#include "ports.h"
#include "terminal.h"
#include "buffer.h"
#include "libc/stdio.h"
#include "ps2.h"
#include "sys.h"
#include "heap.h"
#include <stdint.h>

/*
 * Scan code   Key                         Scan code   Key                     Scan code   Key                     Scan code   Key
 * 0x01        escape pressed              0x02        1 pressed               0x03        2 pressed
 * 0x04        3 pressed                   0x05        4 pressed               0x06        5 pressed               0x07        6 pressed
 * 0x08        7 pressed                   0x09        8 pressed               0x0A        9 pressed               0x0B        0 (zero) pressed
 * 0x0C        - pressed                   0x0D        = pressed               0x0E        backspace pressed       0x0F        tab pressed
 * 0x10        Q pressed                   0x11        W pressed               0x12        E pressed               0x13        R pressed
 * 0x14        T pressed                   0x15        Y pressed               0x16        U pressed               0x17        I pressed
 * 0x18        O pressed                   0x19        P pressed               0x1A        [ pressed               0x1B        ] pressed
 * 0x1C        enter pressed               0x1D        left control pressed    0x1E        A pressed               0x1F        S pressed
 * 0x20        D pressed                   0x21        F pressed               0x22        G pressed               0x23        H pressed
 * 0x24        J pressed                   0x25        K pressed               0x26        L pressed               0x27        ; pressed
 * 0x28        ' (single quote) pressed    0x29        ` (back tick) pressed   0x2A        left shift pressed      0x2B        \ pressed
 * 0x2C        Z pressed                   0x2D        X pressed               0x2E        C pressed               0x2F        V pressed
 * 0x30        B pressed                   0x31        N pressed               0x32        M pressed               0x33        , pressed
 * 0x34        . pressed                   0x35        / pressed               0x36        right shift pressed     0x37        (keypad) * pressed
 * 0x38        left alt pressed            0x39        space pressed           0x3A        CapsLock pressed        0x3B        F1 pressed
 * 0x3C        F2 pressed                  0x3D        F3 pressed              0x3E        F4 pressed              0x3F        F5 pressed
 * 0x40        F6 pressed                  0x41        F7 pressed              0x42        F8 pressed              0x43        F9 pressed
 * 0x44        F10 pressed                 0x45        NumberLock pressed      0x46        ScrollLock pressed      0x47        (keypad) 7 pressed
 * 0x48        (keypad) 8 pressed          0x49        (keypad) 9 pressed      0x4A        (keypad) - pressed      0x4B        (keypad) 4 pressed
 * 0x4C        (keypad) 5 pressed          0x4D        (keypad) 6 pressed      0x4E        (keypad) + pressed      0x4F        (keypad) 1 pressed
 * 0x50        (keypad) 2 pressed          0x51        (keypad) 3 pressed      0x52        (keypad) 0 pressed      0x53        (keypad) . pressed
 * 0x57        F11 pressed                 0x58        F12 pressed
 */

// Scancode -> ASCII
const uint8_t lower_ascii_codes[256] = {
    0x00,  ESC,  '1',  '2',     /* 0x00 */
     '3',  '4',  '5',  '6',     /* 0x04 */
     '7',  '8',  '9',  '0',     /* 0x08 */
     '-',  '=',   BS, '\t',     /* 0x0C */
     'q',  'w',  'e',  'r',     /* 0x10 */
     't',  'y',  'u',  'i',     /* 0x14 */
     'o',  'p',  '[',  ']',     /* 0x18 */
    '\n', 0x00,  'a',  's',     /* 0x1C */
     'd',  'f',  'g',  'h',     /* 0x20 */
     'j',  'k',  'l',  ';',     /* 0x24 */
    '\'',  '`', 0x00, '\\',     /* 0x28 */
     'z',  'x',  'c',  'v',     /* 0x2C */
     'b',  'n',  'm',  ',',     /* 0x30 */
     '.',  '/', 0x00,  '*',     /* 0x34 */
    0x00,  ' ', 0x00, 0x00,     /* 0x38 */
    0x00, 0x00, 0x00, 0x00,     /* 0x3C */
    0x00, 0x00, 0x00, 0x00,     /* 0x40 */
    0x00, 0x00, 0x00,  '7',     /* 0x44 */
     '8',  '9',  '-',  '4',     /* 0x48 */
     '5',  '6',  '+',  '1',     /* 0x4C */
     '2',  '3',  '0',  '.',     /* 0x50 */
    0x00, 0x00, 0x00, 0x00,     /* 0x54 */
    0x00, 0x00, 0x00, 0x00      /* 0x58 */
};

// Scancode -> ASCII
const uint8_t upper_ascii_codes[256] = {
    0x00,  ESC,  '!',  '@',     /* 0x00 */
     '#',  '$',  '%',  '^',     /* 0x04 */
     '&',  '*',  '(',  ')',     /* 0x08 */
     '_',  '+',   BS, '\t',     /* 0x0C */
     'Q',  'W',  'E',  'R',     /* 0x10 */
     'T',  'Y',  'U',  'I',     /* 0x14 */
     'O',  'P',  '{',  '}',     /* 0x18 */
    '\n', 0x00,  'A',  'S',     /* 0x1C */
     'D',  'F',  'G',  'H',     /* 0x20 */
     'J',  'K',  'L',  ':',     /* 0x24 */
     '"',  '~', 0x00,  '|',     /* 0x28 */
     'Z',  'X',  'C',  'V',     /* 0x2C */
     'B',  'N',  'M',  '<',     /* 0x30 */
     '>',  '?', 0x00,  '*',     /* 0x34 */
    0x00,  ' ', 0x00, 0x00,     /* 0x38 */
    0x00, 0x00, 0x00, 0x00,     /* 0x3C */
    0x00, 0x00, 0x00, 0x00,     /* 0x40 */
    0x00, 0x00, 0x00,  '7',     /* 0x44 */
     '8',  '9',  '-',  '4',     /* 0x48 */
     '5',  '6',  '+',  '1',     /* 0x4C */
     '2',  '3',  '0',  '.',     /* 0x50 */
    0x00, 0x00, 0x00, 0x00,     /* 0x54 */
    0x00, 0x00, 0x00, 0x00      /* 0x58 */
};

// flags
uint8_t shift = 0;
uint8_t ctrl = 0;
uint8_t capslock = 0;
uint8_t numlock = 0;
uint8_t scrolllock = 0;
uint8_t keypresses[256];

//Buffer
#define BUFFLEN 256

buffer_t kbuff;

//Helpers
static char scancode_to_ascii(unsigned char c, int shift);
void keyboard_handler(void);
extern void sys_cli();
extern void sys_sti();
extern void pause();

void init_keyboard() {

    printf("Initializing keyboard.\n");

    outb(0x64, 0xFF);
    uint8_t status = inb(0x64);
    printf("Got status (%x) after reset.\n", status);
    
    status = inb(0x64);
    if(status & (1 << 0)) {
        printf("Output buffer full.\n");
    }
    else {
        printf("Output buffer empty.\n");
    }

    if(status & (1 << 1)) {
        printf("Input buffer full.\n");
    }
    else {
        printf("Input buffer empty.\n");
    }

    if(status & (1 << 2)) {
        printf("System flag set.\n");
    }
    else {
        printf("System flag unset.\n");
    }

    if(status & (1 << 3)) {
        printf("Command/Data -> PS/2 device.\n");
    }
    else {
        printf("Command/Data -> PS/2 controller.\n");
    }

    if(status & (1 << 6)) {
        printf("Timeout error.\n");
    }
    else {
        printf("No timeout error.\n");
    }

    if(status & (1 << 7)) {
        printf("Parity error.\n");
    }
    else {
        printf("No parity error.\n");
    }

    // Test the controller.
    outb(0x64, 0xAA);
    uint8_t result = inb(0x60);
    if(result == 0x55) {
        printf("PS/2 controller test passed.\n");
    }
    else if(result == 0xFC) {
        printf("PS/2 controller test failed.\n");
//        return;
    }
    else {
        printf("PS/2 controller responded to test with unknown code %x\n", result);
        printf("Trying to continue.\n");
//        return;
    }

    // Check the PS/2 controller configuration byte.
    outb(0x64, 0x20);
    result = inb(0x60);
    printf("PS/2 config byte: %x\n", result);

    register_interrupt_handler(IRQ1, keyboard_handler);
    circular_buffer_create(&kbuff, 0);

    printf("Initialize keyboard buffer\n");

    printf("Keyboard ready to go!\n\n");
}

buffer_t* keyboard_get_buffer(void){

    return &kbuff;
}

void keyboard_handler(void){

    unsigned char scancode = inb(PS2_DATA_PORT);

    // Umieszczanie w buforze drukowalnych znaków
    if(lower_ascii_codes[scancode]){

        char c = scancode_to_ascii(scancode, shift | capslock);
        circular_buffer_push_back(&kbuff, c);
    }

}

static char scancode_to_ascii(unsigned char c, int shift){

    return lower_ascii_codes[c];
}