#ifndef __GDT_H__
#define __GDT_H__

#include <stddef.h>
#include <stdint.h>

#define GDT_SIZE 4

//Segment describing structure
typedef struct segment_t{
    uint32_t base;
    uint32_t limit;
    uint8_t access;
    uint8_t type;
} segment_t;


//Structure describing gdt entry
typedef struct gdt_entry_t{
    uint16_t limit_top;
    uint16_t base_top;
    uint8_t base_middle;
    uint8_t access;
    uint8_t limit_bottom :4;
    uint8_t flags :4;
    uint8_t base_bottom;
} gdt_entry_t;

//Table of structures describing sectors
segment_t segments[GDT_SIZE];

//Global descriptors table
gdt_entry_t GDT[GDT_SIZE];

void encodeGdtEntry(gdt_entry_t* target, segment_t* source);
void create_descriptor(uint32_t base, uint32_t limit, uint16_t flag);

#endif