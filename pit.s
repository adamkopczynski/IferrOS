.section .text

.global read_PIT_count
.type read_PIT_count, @function
read_PIT_count:
	pushfd
	cli
	mov al, 00000000b    ; al = channel in bits 6 and 7, remaining bits clear
	out 0x43, al         ; Send the latch command
 
	in al, 0x40          ; al = low byte of count
	mov ah, al           ; ah = low byte of count
	in al, 0x40          ; al = high byte of count
	rol ax, 8            ; al = low byte, ah = high byte (ax = current count)
	popfd
	ret

.global set_PIT_count
.type set_PIT_count, @function
set_PIT_count:
	pushfd
	cli
	out 0x40, al        ; Set low byte of reload value
	rol ax, 8           ; al = high byte, ah = low byte
	out 0x40, al        ; Set high byte of reload value
	rol ax, 8           ; al = low byte, ah = high byte (ax = original reload value)
	popfd
	ret