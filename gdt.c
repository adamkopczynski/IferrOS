#include <stddef.h>
#include <stdint.h>
#include "gdt.h"
#include "libc/stdio.h"

void gdt_initialize(void){

    printf("Initialize GDT\n");
    
    //Non-used sector
    segments[0] = (segment_t){
        .base = 0,
        .limit = 0,
        .access = 0,
        .flags = FLAG_32BIT_MODE
    };

    //CS - Code Segment
    segments[1] = (segment_t){
        .base = 0,
        .limit = 0xFFFFFFFF,
        .access = ACCESS_PRESENT | ACCESS_RING0 | ACCESS_CD_SEGMENT | ACCESS_EXECUTABLE | ACCESS_READ_WRITE, 
        .flags = FLAG_32BIT_MODE | FLAG_4KB_GRANULARITY
    };

    //DS - Data Segment
    segments[2] = (segment_t){
        .base = 0,
        .limit = 0xFFFFFFFF,
        .access = ACCESS_PRESENT | ACCESS_RING0 | ACCESS_CD_SEGMENT | ACCESS_EXECUTABLE | ACCESS_READ_WRITE,
        .flags = FLAG_32BIT_MODE | FLAG_4KB_GRANULARITY
    };

    //TSS - empty for now
    segments[3] = (segment_t){
        .base = 0,
        .limit = 0,
        .access = 0, //Add access flags
        .flags = FLAG_32BIT_MODE
    };

    //Create global descriptors table
    for(int i = 0; i < 4; i++){
        encode_gdt_entry(GDT+i, segments+i);
    }

    //Load selectors to register
    reload_segments();

    //Load GDT to GDR
    setGdt((uint32_t)GDT, sizeof(GDT)-1);
}

void encode_gdt_entry(gdt_entry_t* target, segment_t* source){   
    
    //Base
    target->base_low = source->base & 0x0000FFFF;
    target->base_middle = (source->base && 0x00FF0000) >> 16;
    target->base_high = (source->base && 0xFF000000) >> 24;

    //Limit

    target->limit_low = source->limit && 0x0000FFFF;
    target->limit_high = (source->limit && 0x000F0000) >> 16; // limit_hight has only 4 bytes

    //Access
    target->access = source->access;

    //Flags
    target->flags = source->flags;

}