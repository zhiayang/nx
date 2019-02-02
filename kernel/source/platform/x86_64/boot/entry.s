// entry.s
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

.code64

.extern kernel_main

.global kernel_entry
kernel_entry:
	cli

	// Load Long Mode GDT
	mov GDT64Pointer, %rax
	lgdt GDT64Pointer

	// efx calls us with a pointer to the bootinfo struct.
	// this should be in %rdi. we do not touch that, so it should not be trashed.

	// Set up segments
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %gs
	mov %ax, %ss

	movq $StackEnd, %rsp
	movq $0x0, %rbp


	// Setup SSE
	mov %cr0, %rax
	and $0xFFFB, %ax        // clear coprocessor emulation CR0.EM
	or $0x02, %ax           // set coprocessor monitoring  CR0.MP
	mov %rax, %cr0
	mov %cr4, %rax
	orq $0x10600, %rax      // set CR4.OSFXSR, CR4.OSXMMEXCPT and CR4.FSGSBASE at the same time
	orq $0x600, %rax
	mov %rax, %cr4



	// clear the wp bit in cr0 so ring0 can read/write all pages (without needing PAGE_WRITE)
	mov %cr0, %eax
	andl $0xfffeffff, %eax
	mov %eax, %cr0


	// 'far return' to the kernel -- changing the code segment from whatever nonsense
	// uefi set up to our proper one.

	pushq $0x10         // stack segment
	pushq %rsp          // stack pointer
	pushfq              // flags
	pushq $0x08         // code segment
	pushq $kernel_premain  // return address

	iretq

	// unreachable!
	ud2


kernel_premain:
	// setup a null return address
	xor %rbp, %rbp

	// stack should be 16-byte aligned on entry into a function
	// but, the function will push %rbp as part of its prologue
	// so, we make sure the stack is *not* 16-byte aligned (ie it is 0x......8)

	mov %rsp, %rax
	and $0xfffffffffffffff0, %rax
	cmp %rax, %rsp

	jne skip_align

	sub $8, %rsp

skip_align:

	jmp _ZN2nx4initEPNS_8BootInfoE





.section .data
.align 16


GDT64:
GDTNull:
	.word 0         // Limit (low)
	.word 0         // Base (low)
	.byte 0         // Base (middle)
	.byte 0         // Access
	.byte 0         // Granularity / Limit (high)
	.byte 0         // Base (high)
GDTCode:
	.word 0xFFFF    // Limit (low)
	.word 0         // Base (low)
	.byte 0         // Base (middle)
	.byte 0x9A      // Access
	.byte 0xAF      // Granularity / Limit (high)
	.byte 0         // Base (high)
GDTData:
	.word 0xFFFF    // Limit (low)
	.word 0         // Base (low)
	.byte 0         // Base (middle)
	.byte 0x92      // Access
	.byte 0xAF      // Granularity / Limit (high)
	.byte 0         // Base (high)


// Pointer
GDT64Pointer:
	.word GDT64Pointer - GDT64 - 1  // Limit
	.quad GDT64                     // Base


// temp stack for use before we set up threading
// note: we fill with 0xCC, like on windows for unintialised stack memory.
// we might want to remove this eventually (if we get confident in the kernel's stability)
// for faster loading time, since it can be part of the BSS if it's zero.

Stack:

	.align 16
	.space 0x8000, 0xCC

StackEnd:











