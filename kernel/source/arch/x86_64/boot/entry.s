// entry.s
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

.code64

.extern kernel_main

.global kernel_entry
kernel_entry:
	cli

	// never gets old
	xchg %bx, %bx




	// Load Long Mode GDT
	mov GDT64Pointer, %rax
	lgdt GDT64Pointer

	// efx calls us with a pointer to the bootinfo struct.
	// this should be in %rdi. we do not touch that, so it should not be trashed.

	// Set up segments
	mov $0x0, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %gs
	mov %ax, %fs

	// stack segment is a special fucker
	mov $0x10, %ax
	mov %ax, %ss


	movq $StackEnd, %rsp
	movq $0x0, %rbp




	// Setup SSE
	mov %cr0, %rax
	and $0xFFFB, %ax        // clear coprocessor emulation CR0.EM
	or $0x02, %ax           // set coprocessor monitoring  CR0.MP
	mov %rax, %cr0
	mov %cr4, %rax
	orq $0x600, %rax        // set CR4.OSFXSR and CR4.OSXMMEXCPTe
	mov %rax, %cr4


enable_nx_bit:
	jmp 1f

	// enable the nx-bit

	// first, see if it's disabled (see https://forum.osdev.org/viewtopic.php?f=1&t=18945)
	// and http://sandpile.org/x86/msr.htm -- we want MSR MISC_ENABLE, 0x1A0, and unset bit 34.
	movl $0x1A0, %ecx
	rdmsr


	// since it's bit 34, we just use edx.
	andl $0xfffffffb, %edx
	wrmsr


1:
	// next, check if we even support it.
	movl $0x80000001, %eax
	cpuid

	andl $(1 << 20), %edx
	jz 1f


	// we do. set bit 11 of the EFER MSR (0xC0000080)
	mov $0xC0000080, %ecx
	rdmsr

	orl $(1 << 11), %eax
	wrmsr

	movq $1, __nx_x64_was_nx_bit_enabled
	jmp 2f

1:
	movq $0, __nx_x64_was_nx_bit_enabled

2:
	// now we need to check for cmpxchg16b.
	// it's in function 01, ecx bit 13
	movl $0x1, %eax
	cpuid

	andl $(1 << 13), %ecx
	jz 1f

	movq $1, __nx_x64_have_cmpxchg16b
	jmp 2f
1:
	movq $0, __nx_x64_have_cmpxchg16b
2:
	jmp goto_kernel











goto_kernel:
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
	pushq %rbp

	// the stack should be 16-byte aligned on the "call" instruction; the call will push a return
	// address, which leads to it being 8-bytes off. but, since we are using a jmp and not a call,
	// we need to make sure the stack is "already" 8-bytes off. so just force align and subtract.

	and $0xfffffffffffffff0, %rsp
	sub $8, %rsp

	jmp _ZN2nx4initEPNS_8BootInfoE





.section .data

.global __nx_x64_was_nx_bit_enabled
__nx_x64_was_nx_bit_enabled:
	.quad 0

.global __nx_x64_have_cmpxchg16b
__nx_x64_have_cmpxchg16b:
	.quad 0



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











