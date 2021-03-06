#ifndef __GDT_H__
#define __GDT_H__

#include <stddef.h>
#include <stdint.h>

//Structure describing segment
// This structure contains the value of one GDT entry.
// We use the attribute 'packed' to tell GCC not to change
// any of the alignment in the structure.
struct gdt_entry_struct
{
    uint16_t limit_low;           // The lower 16 bits of the limit.
    uint16_t base_low;            // The lower 16 bits of the base.
    uint8_t  base_middle;         // The next 8 bits of the base.
    uint8_t  access;              // Access flags, determine what ring this segment can be used in.
    uint8_t  granularity;         // low 4 bits are high 4 bits of limit
    uint8_t  base_high;           // The last 8 bits of the base.
} __attribute__((packed));
typedef struct gdt_entry_struct gdt_entry_t;

struct gdt_ptr_struct
{
    uint16_t limit;               // The upper 16 bits of all selector limits.
    uint32_t base;                // The address of the first gdt_entry_t struct.
}
    __attribute__((packed));
typedef struct gdt_ptr_struct gdt_ptr_t;

// Lets us access our ASM functions from our C code.
//extern void load_gdt(uint32_t);
extern void load_gdt(gdt_ptr_t * gdt_ptr);

void init_gdt(void);

#endif