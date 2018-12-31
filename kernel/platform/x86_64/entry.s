// entry.s
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

.code64

.extern kernel_main

.global kernel_entry
kernel_entry:
	// Load Long Mode GDT
	mov GDT64Pointer, %rax
	lgdt GDT64Pointer

// efx calls us with a pointer to the bootinfo struct.
// this should be in %rdi. we do not touch that, so it should not be trashed.

kernel_entry2:
	// Set up segments
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %gs

	cli

	// call the kernel.
	call kernel_main

loop:
	jmp loop



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











