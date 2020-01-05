.section .text

.global read_PIT_count
.type read_PIT_count, @function
read_PIT_count:
	pushf							
	cli									
	mov $0, %al   
	out %al, $0x43   					    
	in $0x40, %al        		
	mov %al, %ah     			
	in $0x40, %al       			
	rol $8, %ax  
	sti       
	popf
	ret


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