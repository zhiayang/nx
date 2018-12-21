// longjmp.s
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


.global siglongjmp
.type siglongjmp, @function

siglongjmp:
	jmp longjmp



.global longjmp
.type longjmp, @function

longjmp:
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

	/*
		procedure:

		restore all registers from provided structure.
		(all GPRs, %rsp, %rbp, %rflags)

		push return address
		ret.
	*/



	// firstly check if we need to modify our return, which is stored in %rsi.
	cmpq $0, %rsi
	jne restore

	movq $1, ModifyReturn


restore:
	movq %rsi, ModifyReturn
	movq (0 * 8)(%rdi),	%rdi
	movq (1 * 8)(%rdi),	%rsi
	movq (2 * 8)(%rdi),	%rbp

	movq (3 * 8)(%rdi),	%rax
	movq (4 * 8)(%rdi),	%rbx
	movq (5 * 8)(%rdi),	%rcx
	movq (6 * 8)(%rdi),	%rdx

	movq (7 * 8)(%rdi),	%r8
	movq (8 * 8)(%rdi),	%r9
	movq (9 * 8)(%rdi),	%r10
	movq (10 * 8)(%rdi),	%r11
	movq (11 * 8)(%rdi),	%r12
	movq (12 * 8)(%rdi),	%r13
	movq (13 * 8)(%rdi),	%r14
	movq (14 * 8)(%rdi),	%r15
	movq (15 * 8)(%rdi),	%rsp

	// restore flags first
	pushq (17 * 8)(%rdi)
	popfq

	// since in setjmp we set the %rsp to the current state without pushing anything, (%rsp) is the return address we need to modify.
	movq (16 * 8)(%rdi),	%rax
	movq %rax,		(%rsp)


	// finally, forge our return value.
	movq ModifyReturn,	%rax

	ret

.section .data
ModifyReturn:
	.quad	0


