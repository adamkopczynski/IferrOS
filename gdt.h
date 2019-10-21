#ifndef __GDT_H__
#define __GDT_H__

#include <stddef.h>
#include <stdint.h>

#define GDT_SIZE 4

//Access Flags
#define ACCESS_ACCESSED 0x01
#define ACCESS_READ_WRITE 0x02
#define ACCESS_DIRECTION 0x04
#define ACCESS_EXECUTABLE 0x08
#define ACCESS_CD_SEGMENT 0x10
//Privilege - ring level
#define ACCESS_RING0 0x00
#define ACCESS_RING1 0x20
#define ACCESS_RING2 0x40
#define ACCESS_RING3 0xC0
#define ACCESS_PRESENT 0x80

//Flags
#define FLAG_32BIT_MODE 0x4
#define FLAG_4KB_GRANULARITY 0x8

//Structure describing segment
typedef struct segment_t{
    uint32_t base;
    uint32_t limit;
    uint8_t access;
    uint8_t flags;
} segment_t;


//Structure describing gdt entry
typedef struct gdt_entry_t{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t limit_high :4;
    uint8_t flags :4;
    uint8_t base_high;
} gdt_entry_t;

//Table of structures describing sectors
segment_t segments[GDT_SIZE];

//Global descriptors table
gdt_entry_t GDT[GDT_SIZE];

void gdt_initialize(void);
void encode_gdt_entry(gdt_entry_t* target, segment_t* source);
void create_descriptor(uint32_t base, uint32_t limit, uint16_t flag);
extern void setGdt(uint32_t gdt, uint16_t gdt_size);
extern void reload_segments(void);

#endif