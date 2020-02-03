#include "idt.h"
#include "pic.h"
#include "stdint.h"
#include "libc/stdio.h"

// These extern directives let us access the addresses of our ASM irq handlers.
extern void irq0 ();
extern void irq1 ();
extern void irq2 ();
extern void irq3 ();
extern void irq4 ();
extern void irq5 ();
extern void irq6 ();
extern void irq7 ();
extern void irq8 ();
extern void irq9 ();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();
extern void irq16();
extern void irq17();
extern void irq18();
extern void irq19();
extern void irq20();
extern void irq21();
extern void irq22();
extern void irq23();
extern void irq24();
extern void irq25();
extern void irq26();
extern void irq27();
extern void irq28();
extern void irq29();
extern void irq30();
extern void irq31();


// IRQ Handlers
extern void irq32();
extern void irq33();
extern void irq34();
extern void irq35();
extern void irq36();
extern void irq37();
extern void irq38();
extern void irq39();
extern void irq40();
extern void irq41();
extern void irq42();
extern void irq43();
extern void irq44();
extern void irq45();
extern void irq46();
extern void irq47();

extern void load_idt(uint32_t, uint16_t);
static void idt_set_gate(uint8_t,uint32_t,uint16_t,uint8_t);

idt_ptr_t idt_ptr;

void init_idt(void){

    PIC_remap(32, 40);

    idt_ptr.limit = sizeof(idt_entry_t) * 256 -1;
    idt_ptr.base  = (uint32_t)&IDT;

    idt_set_gate( 0, (uint32_t)irq0 , 0x08, 0x8E);
    idt_set_gate( 1, (uint32_t)irq1 , 0x08, 0x8E);
    idt_set_gate( 2, (uint32_t)irq2, 0x08, 0x8E);
    idt_set_gate( 3, (uint32_t)irq3, 0x08, 0x8E);
    idt_set_gate( 4, (uint32_t)irq4, 0x08, 0x8E);
    idt_set_gate( 5, (uint32_t)irq5, 0x08, 0x8E);
    idt_set_gate( 6, (uint32_t)irq6, 0x08, 0x8E);
    idt_set_gate( 7, (uint32_t)irq7, 0x08, 0x8E);
    idt_set_gate( 8, (uint32_t)irq8, 0x08, 0x8E);
    idt_set_gate( 9, (uint32_t)irq9, 0x08, 0x8E);
    idt_set_gate(10, (uint32_t)irq10, 0x08, 0x8E);
    idt_set_gate(11, (uint32_t)irq11, 0x08, 0x8E);
    idt_set_gate(12, (uint32_t)irq12, 0x08, 0x8E);
    idt_set_gate(13, (uint32_t)irq13, 0x08, 0x8E);
    idt_set_gate(14, (uint32_t)irq14, 0x08, 0x8E);
    idt_set_gate(15, (uint32_t)irq15, 0x08, 0x8E);
    idt_set_gate(16, (uint32_t)irq16, 0x08, 0x8E);
    idt_set_gate(17, (uint32_t)irq17, 0x08, 0x8E);
    idt_set_gate(18, (uint32_t)irq18, 0x08, 0x8E);
    idt_set_gate(19, (uint32_t)irq19, 0x08, 0x8E);
    idt_set_gate(20, (uint32_t)irq20, 0x08, 0x8E);
    idt_set_gate(21, (uint32_t)irq21, 0x08, 0x8E);
    idt_set_gate(22, (uint32_t)irq22, 0x08, 0x8E);
    idt_set_gate(23, (uint32_t)irq23, 0x08, 0x8E);
    idt_set_gate(24, (uint32_t)irq24, 0x08, 0x8E);
    idt_set_gate(25, (uint32_t)irq25, 0x08, 0x8E);
    idt_set_gate(26, (uint32_t)irq26, 0x08, 0x8E);
    idt_set_gate(27, (uint32_t)irq27, 0x08, 0x8E);
    idt_set_gate(28, (uint32_t)irq28, 0x08, 0x8E);
    idt_set_gate(29, (uint32_t)irq29, 0x08, 0x8E);
    idt_set_gate(30, (uint32_t)irq30, 0x08, 0x8E);
    idt_set_gate(31, (uint32_t)irq31, 0x08, 0x8E);


    // IRQ entries
    idt_set_gate(32, (uint32_t)irq32, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t)irq33, 0x08, 0x8E);
    idt_set_gate(34, (uint32_t)irq34, 0x08, 0x8E);
    idt_set_gate(35, (uint32_t)irq35, 0x08, 0x8E);
    idt_set_gate(36, (uint32_t)irq36, 0x08, 0x8E);
    idt_set_gate(37, (uint32_t)irq37, 0x08, 0x8E);
    idt_set_gate(38, (uint32_t)irq38, 0x08, 0x8E);
    idt_set_gate(39, (uint32_t)irq39, 0x08, 0x8E);
    idt_set_gate(40, (uint32_t)irq40, 0x08, 0x8E);
    idt_set_gate(41, (uint32_t)irq41, 0x08, 0x8E);
    idt_set_gate(42, (uint32_t)irq42, 0x08, 0x8E);
    idt_set_gate(43, (uint32_t)irq43, 0x08, 0x8E);
    idt_set_gate(44, (uint32_t)irq44, 0x08, 0x8E);
    idt_set_gate(45, (uint32_t)irq45, 0x08, 0x8E);
    idt_set_gate(46, (uint32_t)irq46, 0x08, 0x8E);
    idt_set_gate(47, (uint32_t)irq47, 0x08, 0x8E);

    printf("Setting IDTR Register\n");
    uint32_t addres = (uint32_t)IDT;
    uint16_t size = (uint16_t)(sizeof(IDT)*256-1);
    load_idt(addres, size);
}

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    IDT[num].offset_lowerbits = base & 0xFFFF;
    IDT[num].offset_higherbits = (base >> 16) & 0xFFFF;

    IDT[num].selector = sel;
    IDT[num].zero = 0;
    // We must uncomment the OR below when we get to using user-mode.
    // It sets the interrupt gate's privilege level to 3.
    IDT[num].type_attr   = flags /* | 0x60 */;
}