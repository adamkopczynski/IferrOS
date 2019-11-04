// #include "idt.h"
// #include "pic.h"

// void idt_init(void) {
              
// 	unsigned long idt_address;
// 	unsigned long idt_ptr[2];
 
//     /* remapping the PIC */
// 	PIC_remap(32, 40);

//     //Helper irqs array
//     void (*irqs[IRQS_LENGTH])(void) = {
//         irq1, irq2, irq3, irq4, irq5, irq6, irq7, irq8, irq9, irq10, irq11, irq12, irq13,
//         irq14, irq15, irq16, irq17, irq18, irq19, irq20, irq21, irq22, irq23, irq24, irq25, irq26,
//         irq27, irq28, irq29, irq30, irq31, irq32, irq33, irq34, irq35, irq36, irq37, irq38, irq39, 
//         irq40, irq41, irq42, irq43, irq44, irq45, irq46, irq47
//     };

//     for(int i = 0; i < IRQS_LENGTH; i++){
//         set_IDT_entry(IDT+i, (uint32_t)*(irqs+i), SELECTOR, TYPE);
//     }

 
// 	/* fill the IDT descriptor */
// 	idt_address = (unsigned long)IDT ;
// 	idt_ptr[0] = (sizeof (struct IDT_entry) * 256) + ((idt_address & 0xffff) << 16);
// 	idt_ptr[1] = idt_address >> 16 ;
 
// 	load_idt(idt_ptr);
 
// }

// void set_IDT_entry(struct IDT_entry *entry, uint32_t address, uint16_t selector, uint8_t type){

// 	entry->offset_lowerbits = address & 0xffff;
// 	entry->selector = selector; /* KERNEL_CODE_SEGMENT_OFFSET */
// 	entry->zero = 0;
// 	entry->type_attr = type; /* INTERRUPT_GATE */
// 	entry->offset_higherbits = (address & 0xffff0000) >> 16;
// }

// void interrupt_handler(uint32_t nr){

//     uint32_t count = handlers[nr].count;

//     for(uint32_t i=0; i<count; i++){

//         HANDLER_FUN handler = handlers[nr].funs[i];
//         handler();
//     }

//     PIC_sendEOI(nr);
// }