// macros.s
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.



.macro push_all_regs
	push %r15
	push %r14
	push %r13
	push %r12
	push %r11
	push %r10
	push %r9
	push %r8
	push %rdx
	push %rcx
	push %rbx
	push %rax
	push %rbp
	push %rsi
	push %rdi
.endm

.macro pop_all_regs
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
.endm

.macro push_scratch_regs
	push %r11
	push %r10
	push %r9
	push %r8
	push %rdx
	push %rcx
	push %rax
	push %rsi
	push %rdi
.endm

.macro pop_scratch_regs
	pop %rdi
	pop %rsi
	pop %rax
	pop %rcx
	pop %rdx
	pop %r8
	pop %r9
	pop %r10
	pop %r11
.endm









