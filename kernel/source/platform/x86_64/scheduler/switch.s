// switch.s
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

.section .text

.global nx_x64_switch_to_thread
.type nx_x64_switch_to_thread, @function


// parameters: new rsp (%rdi), new cr3 (%rsi)
nx_x64_switch_to_thread:

	// if the new cr3 is 0, then we do not switch.
	cmp $0, %rsi
	je restore_registers

change_cr3:
	mov %rsi, %cr3

restore_registers:
	mov %rdi, %rsp

	pop %rdi
	pop %rsi
	pop %rbp
	pop %rax
	pop %rbx
	pop %rcx
	pop %rdx
	pop %r8
	pop %r9
	pop %r10
	pop %r11
	pop %r12
	pop %r13
	pop %r14
	pop %r15

	iretq













