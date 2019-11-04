/* Declare constants for the multiboot header. */
.set ALIGN,    1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,  1<<1             /* provide memory map */
.set FLAGS,    ALIGN | MEMINFO  /* this is the Multiboot 'flag' field */
.set MAGIC,    0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */
 
/* 
Declare a multiboot header that marks the program as a kernel. These are magic
values that are documented in the multiboot standard. The bootloader will
search for this signature in the first 8 KiB of the kernel file, aligned at a
32-bit boundary. The signature is in its own section so the header can be
forced to be within the first 8 KiB of the kernel file.
*/
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

//Create stack of 16 KiB size
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

//GDTR
gdtr:
.word 0
.long 0

//IDTR
ldtr:
.word 0
.long 0

//Multiboot structure address
multiboot_info:
.long 0

.text

.global get_multibot_info
.type get_multibot_info, @function
get_multibot_info:
	movl multiboot_info, %eax
	ret

//Call from C as setGdt(gdt, sizeof(gdt)-1)
.global setGdt
.type setGdt, @function
setGdt:
	mov   4(%esp), %eax
   	mov   %eax, gdtr+2
   	mov   8(%esp), %ax
   	mov   %ax, gdtr
   	lgdt  gdtr
   	ret
 
.global reload_segments   
.type reload_segments, @function
reload_segments:
	ljmp   $0x8, $reload_CS
.global reload_CS
reload_CS:
   MOV   $0x10, %ax
   MOV   %ax, %ds
   MOV   %ax, %es
   MOV   %ax, %fs
   MOV   %ax, %gs
   MOV   %ax, %ss
   RET


.type enter_protection_mode, @function
enter_protected_mode:
	mov	%cr0, %eax
	or	$0x01, %eax
	mov	%eax, %cr0
	ret
 
// Wykonywanie rozpoczyna się od tej procedury
.global _start
.type _start, @function			
_start:
	mov %ebx, multiboot_info
	cli 									
	call enter_protected_mode		
	mov $stack_top, %esp		  		
	call kernel_main			   
			
1:	hlt     				  			
	jmp 1b							
 
.size _start, . - _start

