.align 4
.section .text
.extern interrupt_default_handler

/* Interrupts */
.macro interrupt_handler interrupt_number
.global interrupt_\interrupt_number
interrupt_\interrupt_number :
	push $0xFEEEFEEE
		push %esp
			pushal
				push %ds
				push %es
				push %fs
				push %gs
					push %esp
					push %esp
						mov $0x10, %eax
						mov %eax, %ds
						mov %eax, %es
						mov $0x2, %eax
						push %eax
							mov $\interrupt_number, %eax
							push %eax
								cld
								call interrupt_default_handler
							pop %eax
						pop %eax
					pop %esp
					pop %esp
				pop %gs
				pop %fs
				pop %es
				pop %ds
			popal
		/* pop %esp */
	add $8, %esp
    iret
.endm

/* Exceptions */
.macro exception_handler interrupt_number
.global interrupt_\interrupt_number
interrupt_\interrupt_number :
    push %esp
	/*push $0xBAD0BAD0*/
		pushal
			push %ds
			push %es
			push %fs
			push %gs
				push %esp
				push %esp
					mov $0x10, %eax
					mov %eax, %ds
					mov %eax, %es
					mov $0x1, %eax
					push %eax
						mov $\interrupt_number, %eax
						
						push %eax
							cld
							call interrupt_default_handler
						pop %eax
					pop %eax
				add $8, %esp
			pop %gs
			pop %fs
			pop %es
			pop %ds
		popal
	/*add $4, %esp */
	pop %esp
	add $4, %esp
	iret
.endm

interrupt_handler 0x20
interrupt_handler 0x21
interrupt_handler 0x22
interrupt_handler 0x23
interrupt_handler 0x24
interrupt_handler 0x25
interrupt_handler 0x26
interrupt_handler 0x27
interrupt_handler 0x28
interrupt_handler 0x29
interrupt_handler 0x2A
interrupt_handler 0x2B
interrupt_handler 0x2C
interrupt_handler 0x2D
interrupt_handler 0x2E
interrupt_handler 0x2F
interrupt_handler 0x30
interrupt_handler 0x31
interrupt_handler 0x32
interrupt_handler 0x99

exception_handler 0
exception_handler 1
exception_handler 2
exception_handler 3
exception_handler 4
exception_handler 5
exception_handler 6
exception_handler 7
exception_handler 8
exception_handler 9
exception_handler 10
exception_handler 11
exception_handler 12
exception_handler 13
exception_handler 14
exception_handler 16
exception_handler 17
exception_handler 18
exception_handler 19
