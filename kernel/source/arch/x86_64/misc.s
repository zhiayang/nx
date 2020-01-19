// misc.s
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

// miscellaneous assembly support functions.

.global nx_x64_clear_segments
.type nx_x64_clear_segments, @function
nx_x64_clear_segments:
	xor %ax, %ax
	mov %ax, %ds
	mov %ax, %es
	ret


.global nx_x64_read_msr
.type nx_x64_read_msr, @function
nx_x64_read_msr:
	// %ecx is the msr to read (but the paramter comes in in %rdi)
	// writing to %ecx clears the top 32-bits.
	mov %edi, %ecx

	// ok, read the msr. it returns in %edx:%eax
	rdmsr

	// shift edx by 32
	shl $32, %rdx
	orq %rdx, %rax

	ret

.global nx_x64_write_msr
.type nx_x64_write_msr, @function
nx_x64_write_msr:
	// %ecx is the msr to write (but the paramter comes in in %rdi)
	// writing to %ecx clears the top 32-bits.
	mov %edi, %ecx

	// %rsi is the value to write, but wrmsr wants it in %edx:%eax
	mov %esi, %eax
	mov %rsi, %rdx
	shr $32, %rdx

	wrmsr
	ret


.global nx_x64_read_cr0
.type nx_x64_read_cr0, @function
nx_x64_read_cr0:
	movq %cr0, %rax
	ret

.global nx_x64_write_cr0
.type nx_x64_write_cr0, @function
nx_x64_write_cr0:
	movq %rdi, %cr0
	ret


.global nx_x64_read_cr4
.type nx_x64_read_cr4, @function
nx_x64_read_cr4:
	movq %cr4, %rax
	ret

.global nx_x64_write_cr4
.type nx_x64_write_cr4, @function
nx_x64_write_cr4:
	movq %rdi, %cr4
	ret
