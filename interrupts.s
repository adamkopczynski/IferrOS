idtr:
.word 0
.long 0

.global load_idt
.type load_idt, @function
    # void load_idt(idt_ptr_t *)
load_idt:
    mov   4(%esp), %eax
    mov   %eax, idtr+2
    mov   8(%esp), %ax
    mov   %ax, idtr
    lidt  idtr
    sti
    ret

.size load_idt, . - load_idt

.extern irq_handler

.macro irq num
.global irq\num
irq\num:
    pushal
    pushl $\num
    call irq_handler
    addl $0x04, %esp
    popal
    iret
.endm

irq 0
irq 1
irq 2
irq 3
irq 4
irq 5
irq 6
irq 7
irq 8
irq 9
irq 10
irq 11
irq 12
irq 13
irq 14
irq 15
irq 16
irq 17
irq 18
irq 19
irq 20
irq 21
irq 22
irq 23
irq 24
irq 25
irq 26
irq 27
irq 28
irq 29
irq 30
irq 31
irq 32
irq 33
irq 34
irq 35
irq 36
irq 37
irq 38
irq 39
irq 40
irq 41
irq 42
irq 43
irq 44
irq 45
irq 46
irq 47