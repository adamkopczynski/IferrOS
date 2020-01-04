/**
 * This file contains the minimal C code for dispatching
 * interrupts to handlers.
 */

#include "isr.h"
#include "idt.h"
#include "pic.h"
#include "ports.h"
#include "sys.h"
#include "libc/stdio.h"
#include <stdint.h>

isr_handler_t interrupt_handlers[256];

void register_interrupt_handler(uint8_t interrupt, isr_handler_t handler)
{
    interrupt_handlers[interrupt] = handler;
}

void isr_handler(registers_t regs)
{
    if(regs.int_no == GENERAL_PROTECTION_FAULT)
    {
        printf("General Protection Fault. Code: %d", regs.err_code);
        kernel_panic("General Protection Fault!");
    }

    if(interrupt_handlers[regs.int_no])
    {
        interrupt_handlers[regs.int_no](regs);
    }
}


void irq_handler(registers_t regs)
{
    //If int_no >= 40, we must reset the slave as well as the master
    if(regs.int_no >= 40)
    {
        //reset slave
        outb(PIC1_CMD, PIC_EOI);
    }

    outb(PIC2_CMD, PIC_EOI);

    if(interrupt_handlers[regs.int_no])
    {
        interrupt_handlers[regs.int_no](regs);
    }
}