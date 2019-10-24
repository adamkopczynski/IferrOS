#ifndef __IDT_H__
#define __IDT_H__

#include <stddef.h>
#include <stdint.h>

struct IDT_entry{
	uint16_t offset_lowerbits;
	uint16_t selector;
	uint8_t zero;
	uint8_t type_attr;
	uint16_t offset_higherbits;
};
 
struct IDT_entry IDT[256];

void idt_init(void);

#endif