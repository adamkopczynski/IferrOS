#include "isr.h"
#include "idt.h"
#include "pic.h"
#include "ports.h"
#include "sys.h"
#include "libc/stdio.h"
#include <stdint.h>

interrupt_handlers_t interrupt_handlers[256];

void register_interrupt_handler(uint8_t interrupt, isr_handler_t handler){

    uint32_t count = interrupt_handlers[interrupt].count;
    if(count == MAX_HANDLERS_COUNT) return;

    interrupt_handlers[interrupt].funs[count] = handler;
    interrupt_handlers[interrupt].count++;
}

void isr_handler(registers_t regs){

    printf("isr handled\n");

    if(regs.int_no == GENERAL_PROTECTION_FAULT){

        printf("General Protection Fault. Code: %d", regs.err_code);
        kernel_panic("General Protection Fault!");
    }

    int count = interrupt_handlers[regs.int_no].count || 0;
    if(count){
       for(int i = 0; i < count; i++){
            interrupt_handlers[regs.int_no].funs[i]();
        }
    }
}


void irq_handler(uint32_t irq){

    //If int_no >= 40, we must reset the slave as well as the master
    if(irq >= 40){
        //reset slave
        PIC_sendEOI(irq);
    }

    int count = interrupt_handlers[irq].count || 0;

    for(int i = 0; i < count; i++){
        isr_handler_t handler = interrupt_handlers[irq].funs[i];
        handler();
    }

    PIC_sendEOI(irq);
}