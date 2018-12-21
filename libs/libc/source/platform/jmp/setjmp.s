// setjmp.s
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

.global sigsetjmp
.type sigsetjmp, @function

sigsetjmp:
	xor %rsi, %rsi
	jmp setjmp


.global setjmp
.type setjmp, @function

setjmp:
	/*
		%rdi is a *POINTER* to a jmp_buf struct.
		uint64_t rdi;		+0
		uint64_t rsi;		+8
		uint64_t rbp;		+16

		uint64_t rax;		+24
		uint64_t rbx;		+32
		uint64_t rcx;		+40
		uint64_t rdx;		+48

		uint64_t r8;		+56
		uint64_t r9;		+64
		uint64_t r10;		+72
		uint64_t r11;		+80
		uint64_t r12;		+88
		uint64_t r13;		+96
		uint64_t r14;		+104
		uint64_t r15;		+112

		uint64_t ret;		+120
		uint64_t rsp;		+128
		uint64_t rflags;	+136
	*/
	movq %rdi,	(0 * 8)(%rdi)
	movq %rsi,	(1 * 8)(%rdi)
	movq %rbp,	(2 * 8)(%rdi)

	movq %rax,	(3 * 8)(%rdi)
	movq %rbx,	(4 * 8)(%rdi)
	movq %rcx,	(5 * 8)(%rdi)
	movq %rdx,	(6 * 8)(%rdi)

	movq %r8,	(7 * 8)(%rdi)
	movq %r9,	(8 * 8)(%rdi)
	movq %r10,	(9 * 8)(%rdi)
	movq %r11,	(10 * 8)(%rdi)
	movq %r12,	(11 * 8)(%rdi)
	movq %r13,	(12 * 8)(%rdi)
	movq %r14,	(13 * 8)(%rdi)
	movq %r15,	(14 * 8)(%rdi)
	movq %rsp,	(15 * 8)(%rdi)

	// we also need to store the return address, which is at the top of the stack currently.
	// don't need to save %rax, since it'll be zero when returning.

	mov (%rsp),	%rax
	mov %rax,	(16 * 8)(%rdi)

	// special: we can't exactly move rflags, so we need to push it, then move it, then pop it.

	pushfq
	mov (%rsp),	%rax
	addq $8, %rsp			// remove rflags

	mov %rax,	(17 * 8)(%rdi)
	xor %rax, %rax

	ret





