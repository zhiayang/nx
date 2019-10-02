// trampoline.s
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

.global kernel_trampoline
.global kernel_trampoline_end

kernel_trampoline:
	// switch cr3.
	mov %rdi, %rax
	mov %rax, %cr3

	// we got entry as the 2nd param, and bootinfo as the 3rd param.
	mov %rsi, %rax

	// kernel expects bootinfo as first param.
	mov %rdx, %rdi

	// call the kernel entry.
	jmp *%rax

kernel_trampoline_end:
	nop
