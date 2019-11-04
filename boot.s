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
	gdtr DW 0 ; For limit storage
     	DD 0 ; For base storage

//IDTR
	idtr DW 0 ; For limit storage
     	DD 0 ; For base storage

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
	MOV   EAX, [esp + 4]
	MOV   [gdtr + 2], EAX
	MOV   AX, [ESP + 8]
	MOV   [gdtr], AX
	LGDT  [gdtr]
	RET
 
/*
The linker script specifies _start as the entry point to the kernel and the
bootloader will jump to this position once the kernel has been loaded. It
doesn't make sense to return from this function as the bootloader is gone.
*/
.section .text
.global _start
.type _start, @function
_start:
	/*
	The bootloader has loaded us into 32-bit protected mode on a x86
	machine. Interrupts are disabled. Paging is disabled. The processor
	state is as defined in the multiboot standard. The kernel has full
	control of the CPU. The kernel can only make use of hardware features
	and any code it provides as part of itself. There's no printf
	function, unless the kernel provides its own <stdio.h> header and a
	printf implementation. There are no security restrictions, no
	safeguards, no debugging mechanisms, only what the kernel provides
	itself. It has absolute and complete power over the
	machine.
	*/
 
	/*
	To set up a stack, we set the esp register to point to the top of the
	stack (as it grows downwards on x86 systems). This is necessarily done
	in assembly as languages such as C cannot function without a stack.
	*/
	mov $stack_top, %esp
 
	//Reload CS register containing code selector:
	.global reload_segments   
	.type reload_segments, @function
	reloadSegments:
		JMP   0x08:reload_CS ; 0x08 points at the new code selector
	.reload_CS:
		; Reload data segment registers:
		MOV   AX, 0x10 ; 0x10 points at the new data selector
		MOV   DS, AX
		MOV   ES, AX
		MOV   FS, AX
		MOV   GS, AX
		MOV   SS, AX
		RET
 
	/*
	Enter the high-level kernel. The ABI requires the stack is 16-byte
	aligned at the time of the call instruction (which afterwards pushes
	the return pointer of size 4 bytes). The stack was originally 16-byte
	aligned above and we've pushed a multiple of 16 bytes to the
	stack since (pushed 0 bytes so far), so the alignment has thus been
	preserved and the call is well defined.
	*/
	call kernel_main
 
	cli
1:	hlt
	jmp 1b
 
/*
Set the size of the _start symbol to the current location '.' minus its start.
This is useful when debugging or when you implement call tracing.
*/
.size _start, . - _start

