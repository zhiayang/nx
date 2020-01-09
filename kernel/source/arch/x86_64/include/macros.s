// macros.s
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.




// note: why this is only called "if necessary":
// if were interrupted while in ring 0 (eg. while running kernel threads), then we *don't want* to swap gs,
// because gsbase will already be valid. similarly, when we are leaving *to* ring 0, we don't want to swap either.

// (we can call this macro on exit as well, but only after restoring the registers -- so the only things on the stack
// are the 5 things that the cpu pushed for the interrupt stack frame)

// on the contrary, if we came from ring 3, cs will be 0x1B, so we will do a swapgs; the same applies when leaving to ring 3.
.macro swapgs_if_necessary offset=0x8
	testb $0x3, \offset(%rsp)
	jz 1f
	swapgs
1:
.endm





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








