.text

.global switch_stacks_and_jump
.global switch_stacks


switch_stacks_and_jump:

	// New Thread
	// Old Thread
	// Return Address
		
	pushf
	pushl %eax	
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %esi
	pushl %edi
	pushl %ebp
	
	mov 36(%esp), %eax
	mov %esp, 0(%eax)

	mov 40(%esp), %eax
    mov 0(%eax), %esp
	mov 8(%eax), %eax

	sti

   jmp *%eax
    

switch_stacks:

	// New Thread
	// Old Thread
	// Return Address

	pushf
	pushl %eax	
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %esi
	pushl %edi
	pushl %ebp

   mov 36(%esp), %eax
	mov %esp, 0(%eax)

	mov 40(%esp), %eax
   mov 0(%eax), %esp

	popl %ebp
   popl %edi
   popl %esi
   popl %edx
   popl %ecx
   popl %ebx
   popl %eax
	popf

	sti

    ret