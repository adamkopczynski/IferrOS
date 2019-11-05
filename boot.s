// Stałe niezbędne dla MULTIBOOT 
.set 		ALIGN,    	0x01        
.set 		MEMINFO,  	0x10             
.set 		FLAGS,    	ALIGN | MEMINFO  
.set 		MAGIC,    	0x1BADB002       
.set 		CHECKSUM, 	-(MAGIC + FLAGS) 
 
// Zapisanie w pamięci wcześniej zdefiniowanych stałych.
// Bootloader szuka wartości MAGIC i odczytuje dwie kolejne 
// Musi znajdować się w pierwszych 8kb programu jądra
.section .multiboot
	.align 4
	.long MAGIC
	.long FLAGS
	.long CHECKSUM
	 
// Tworzy stos o rozmiarze 16kb.
// Wygamane jest aby był wyrówanany do 16 bajtów
// Stos rośnie w dół - rejestr ESP jest dekrementowany
.section .bss
	.align 16
	stack_bottom:
	.skip 16384 # 16 KiB
	stack_top:							

// Rozmiar i adres globalnej tablicy deskryptorów GDT
gdtr:
.word 0
.long 0

// Rozmiar i adres tablicy deskryptorów przerwań IDT
idtr:
.word 0
.long 0

// Adres struktury multiboot
multiboot_info:
.long 0

.text

// Zwraca adres struktury multiboot_info
// Wywoływane z poziomu C - unsigned int get_multibot_info(void)
.global get_multibot_info
.type get_multibot_info, @function
get_multibot_info:
	movl multiboot_info, %eax
	ret

// Ładuje rozmiar i adres globalnej tablicy deskryptorów do rejestru GDTR
// Wywoływane z poziomu C - set_GDT(GDT, sizeof(GDT)-1)
.global set_GDT
.type set_GDT, @function
set_GDT:
   mov   4(%esp), %eax
   mov   %eax, gdtr+2
   mov   8(%esp), %ax
   mov   %ax, gdtr
   lgdt  gdtr
   ret

// Ładuje rozmiar i adres tablicy deskryptorów przerwań do rejestru IDTR
// Wywoływane z poziomu C - set_IDT(IDT, sizeof(IDT)-1)
.global set_IDT
.type set_IDT, @function
set_IDT:
   mov   4(%esp), %eax
   mov   %eax, idtr+2
   mov   8(%esp), %ax
   mov   %ax, idtr
   lidt  idtr
   sti
   ret

// Ładuje selektory do rejestrów segmentowych procesora
// Wywoływanie z poziomu C - reload_segments()
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

// Zwraca stan licznika PIT	
// Wywoływane z poziomu C - unsigned short read_pit_count(void)
.global read_PIT_count
.type read_PIT_count, @function
read_PIT_count:
	pushf									// Zrzucamy na stos rejestr EFLAG, 4 bajty - teraz parametry są głębiej
	cli									
	mov $0, %al   
	out %al, $0x43   					// Polecenie Counter Latch      
	in $0x40, %al        			// Odczyt pierwszego bajtu
	mov %al, %ah     			
	in $0x40, %al       			 	// Odczyt drugiego bajtu 
	rol $8, %ax  
	sti       
	popf
	ret
	
// Ustala stan licznika PIT
// Wywoływane z poziomu C - void set_PIT_count(unsigned short count)
.global set_PIT_count
.type set_PIT_count, @function
set_PIT_count:
	pushf
	cli
	mov 8(%esp), %al
	out %al, $0x40        			// Set low byte of reload value
	rol $8, %ax           			// al = high byte, ah = low byte
	mov 9(%esp), %al
	out %al, $0x40        			// Set high byte of reload value
	rol $8, %ax           			// al = low byte, ah = high byte (ax = original reload value)
	sti
	popf
	ret
   
// Tryb pracy procesora jest zmieniany na tryb chroniony
// Ustawiamy bit PE - pierwszy bit w rejestrze CR0
// Wyłącza to adresowanie płaskie i umożliwia mechanizmy takie jak stronicowanie i segmentacja
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
