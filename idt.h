#ifndef __IDT_H__
#define __IDT_H__

#include <stddef.h>
#include <stdint.h>

#define SELECTOR 0x08
#define TYPE 0x8e

#define IRQS_LENGTH 48

struct IDT_entry{
	uint16_t offset_lowerbits;
	uint16_t selector;
	uint8_t zero;
	uint8_t type_attr;
	uint16_t offset_higherbits;
};
 
struct IDT_entry IDT[256];

void idt_init(void);
void set_IDT_entry(struct IDT_entry *entry, uint32_t address, uint16_t selector, uint8_t type);

extern int load_idt();
extern void irq0(void);  
extern void irq1(void);  
extern void irq2(void);  
extern void irq3(void); 
extern void irq4(void);  
extern void irq5(void); 
extern void irq6(void);  
extern void irq7(void); 
extern void irq8(void);  
extern void irq9(void);  
extern void irq10(void); 
extern void irq11(void); 
extern void irq12(void); 
extern void irq13(void); 
extern void irq14(void); 
extern void irq15(void); 
extern void irq16(void); 
extern void irq17(void); 
extern void irq18(void); 
extern void irq19(void); 
extern void irq20(void);
extern void irq21(void); 
extern void irq22(void); 
extern void irq23(void); 
extern void irq24(void); 
extern void irq25(void); 
extern void irq26(void); 
extern void irq27(void); 
extern void irq28(void);
extern void irq29(void); 
extern void irq30(void); 
extern void irq31(void); 
extern void irq32(void); 
extern void irq33(void); 
extern void irq34(void); 
extern void irq35(void); 
extern void irq36(void); 
extern void irq37(void); 
extern void irq38(void); 
extern void irq39(void); 
extern void irq40(void); 
extern void irq41(void); 
extern void irq42(void); 
extern void irq43(void); 
extern void irq44(void); 
extern void irq45(void); 
extern void irq46(void); 
extern void irq47(void);

#endif