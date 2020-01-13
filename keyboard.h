#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "buffer.h"

#define VK_ENTER 0x1C
#define VK_BACKSPACE 0x0E

void init_keyboard();
buffer_t* keyboard_get_buffer(void);

#endif