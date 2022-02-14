#include "memory.h"

.set MAGIC,    0x1BADB002 

.section .multiboot
.align 4
	.long MAGIC				/* mb magic key */
	.long 0x7				/* Flags: align, mem, video */
	.long -(MAGIC + 0x7)	/* Checksum */

	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	
	#ifdef GRAPHICS_OFF
	.long 1
	.long 80
	.long 25
	.long 0
	#endif
	#ifdef GRAPHICS_ON
	.long 0
	.long 1280
	.long 720
	.long 32
	#endif

.section .versionv_magic, "aw"
	.asciz "VersionV"
	.byte 0x01
	.byte 0x02 
	.byte 0x03
	.byte 0x04

.section .bootstrap_stack, "aw", @nobits
	.align 16
	stack_bottom:
	.skip 16384
	stack_top:

.section .bss, "aw", @nobits 
	.align 4096
	.global boot_page_directory
	.global paging_pd
	paging_pd:
	boot_page_directory:
	.fill 1024, 4, 0

	.global boot_page_table
	.global paging_bpt
	paging_bpt:
	boot_page_table:
	.fill 1024, 4, 0

#ifdef PAGING_PAE
	.align 0x1000
	.global paging_pdpt
	paging_pdpt:
	.fill 4, 8, 0
#endif

.section .data
.global GDTR
.global GDT
GDTR:
	.word	GDT_END - GDT - 1
	.long	GDT
GDT:
NULL_SELECTOR:  	# 0x0
	.long 0x0
	.long 0x0
CODE_SELECTOR:		# 0x08
	.word 0xFFFF
	.word 0x0
	.byte 0x0
	.byte 0x9A
	.byte 0xCF
	.byte 0x0
DATA_SELECTOR:		# 0x10
	.word 0xFFF
	.word 0x0
	.byte 0x0
	.byte 0x92
	.byte 0xCF
	.byte 0x0
CODE_USER:			# 0x18
	.word 0xFFFF
	.word 0x0
	.byte 0x0
	.byte 0xFA
	.byte 0xCF
	.byte 0x0
DATA_USER:			# 0x20
	.word 0xFFFF
	.word 0x0
	.byte 0x0
	.byte 0xF2
	.byte 0xCF
	.byte 0x0
TSS:				# 0x28
	.long 0x0
	.long 0x0
GDT_END:
	
.section .text
.global _asm_kernel_start
.type _asm_kernel_start, @function
_asm_kernel_start:
	#ifdef PAGING_PAE

		movl $(paging_pd - KERNEL_VIRT_LOAD_BASE + 0x1), paging_pdpt - KERNEL_VIRT_LOAD_BASE	    // load PDPT[0] with EDX
		movl $(paging_pd - KERNEL_VIRT_LOAD_BASE + 0x1), paging_pdpt - KERNEL_VIRT_LOAD_BASE + 3*8  // load PDPT[3] with EDX (3 = 3gb = 0xC0000000)
		
		/* Setup the PD with the right address and info */
		/*movl $(paging_pd - KERNEL_VIRT_LOAD_BASE), %edi
		movl 0b10000011, %edx
		movl %edx, (%edi)*/

		xor %ecx, %ecx
		movl $(paging_pd - KERNEL_VIRT_LOAD_BASE), %edi
		movl $0x83, %ecx
		movl %ecx, (%edi)

		movl %cr4, %edx
		bts  $5, %edx
		movl %edx, %cr4

		movl $(paging_pdpt - KERNEL_VIRT_LOAD_BASE), %edi
		movl %edi, %cr3

		movl %cr0, %edx
		orl  $0x80000000, %edx
		
		movl %edx, %cr0

		lea  pae2, %ecx
		jmp  *%ecx										/* BSG: "Jump" */

		pae2:

		lgdt GDTR
		mov $0x10, %ax
		mov %ax, %ds
		mov %ax, %ss
		mov %ax, %es
		mov %ax, %fs 
		mov %ax, %gs

		jmp $0x08, $pae3

		pae3:
		mov  $(stack_top), %esp							/* Setup the stack */

		addl $0xC0000000, %ebx
		push %ebx										/* Multiboot header pointer */
		push %eax										/* Magic number */

		xor %ebp, %ebp									/* NULL Stack for tracing purposes */

		call kernel_main

	#endif
	
	movl $12800, %ecx
	movl $0xFD00c828, %edi
	g1:
	movl $0x00AAAAAA, (%edi)
	addl $4, %edi
	dec %ecx
	cmpl $0, %ecx
	jnz g1
	


	movl $(boot_page_table - 0xC0000000), %edi		/* EDI gets the physical page table  address */
	movl $0, %esi									/* 0 == first address mapped */
	movl $1024, %ecx								/* Map our pages */

stage1:	
	cmpl $0, %ecx
	dec %ecx

stage2:
	movl %esi, %edx									/* Load physical address into EDX */
	orl  $0x003, %edx								/* Make the kernel's phsyical mem present and writeable */
	movl %edx, (%edi)								/* Move table entry to address pointed to in edi */
	addl $4096, %esi								/* Increment esi (physical address) by 4k */
	addl $4, %edi									/* Increment the table entry by 1 (1 entry == 32 bits == 4 bytes) */
	loop stage1
 
stage3:
	/* Setup physical 0 and 0xC0000000 to be present, writable so when we load CR3 we don't error out */
	movl $(boot_page_table - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000
	movl $(boot_page_table - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 768 * 4

	movl $(boot_page_directory - 0xC0000000), %ecx	/* Correct for linker 0xC0000000 offset */
	movl %ecx, %cr3

	movl %cr0, %ecx
	orl  $0x80010000, %ecx							/* Enable w/4kb paging */
	movl %ecx, %cr0


	lea  stage4, %ecx
	jmp  *%ecx										/* BSG: "Jump" */

stage4:
	lgdt GDTR
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs 
	mov %ax, %gs

	jmp $0x08, $stage5

stage5:
	movl $0, boot_page_directory					/* Remove entry 0, since it's mapped now. Refresh cr3 */
	movl %cr3, %ecx
	movl %ecx, %cr3

	mov  $(stack_top), %esp							/* Setup the stack */

	addl $0xC0000000, %ebx
	push %ebx										/* Multiboot header pointer */
	push %eax										/* Magic number */

	xor %ebp, %ebp									/* NULL Stack for tracing purposes */

	call kernel_main

.global asm_refresh_cr3
asm_refresh_cr3:
	xor %ecx, %ecx
	movl %cr3, %ecx
	movl %ecx, %cr3
	ret

.global asm_invalidate_page
asm_invalidate_page:
	movl	(%esp),%eax
	invlpg	(%eax)
	ret

.global get_cr2
get_cr2:
	movl %cr2, %eax
	ret;

.global save_context
save_context:
	push %eax

	addl $8, %esp
	movl (%esp), %eax

	movl %ecx, 0x28(%eax)
	movl -0x4(%esp), %ecx
	movl %ecx, 0x2c(%eax)
	movl 0x28(%eax), %ecx
	
	movl %edx, 0x24(%eax)
	movl %ebx, 0x20(%eax)
	movl %esp, 0x1c(%eax)
	movl %ebp, 0x18(%eax)
	movl %esi, 0x14(%eax)
	movl %edi, 0x10(%eax)

	subl $8, %esp
	pop %eax

	ret

.global restore_context
restore_context:
	addl $4, %esp
	movl (%esp), %eax
	movl -0x4(%esp), %ebx

	movl 0x1c(%eax), %esp
	movl %ebx, -0x8(%esp)

	movl 0x28(%eax), %ecx
	movl 0x24(%eax), %edx
	movl 0x20(%eax), %ebx
	movl 0x18(%eax), %ebp
	movl 0x14(%eax), %esi
	movl 0x10(%eax), %edi
	
	movl 0x2c(%eax), %eax

	subl $4, %esp
	
	ret


.size _asm_kernel_start, . - _asm_kernel_start
