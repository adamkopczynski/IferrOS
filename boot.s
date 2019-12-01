//Multiboot constants
.set 		ALIGN,    	1<<0        
.set 		MEMINFO,  	1<<1            
.set 		FLAGS,    	ALIGN | MEMINFO  
.set 		MAGIC,    	0x1BADB002       
.set 		CHECKSUM, 	-(MAGIC + FLAGS) 
 
.section .multiboot
	.align 4
	.long MAGIC
	.long FLAGS
	.long CHECKSUM
	 
// Create 16kb stack.
.section .bss
	.align 16
	stack_bottom:
	.skip 16384 # 16 KiB
	stack_top:							

// Multiboot address
multiboot_info:
.long 0

.section .text

// Get multiboot info address pointer
.global get_multibot_info
.type get_multibot_info, @function
get_multibot_info:
	movl multiboot_info, %eax
	ret
   
// Change mode to protected
.type enter_protection_mode, @function
enter_protected_mode:
	mov	%cr0, %eax
	or	$0x01, %eax
	mov	%eax, %cr0
	ret
 
//Init point
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
