// Interrupts.s
// Copyright (c) 2013 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

// Defines nice ISRs.

.section .text

.global IRQ_Handler_0
.type IRQ_Handler_0, @function
IRQ_Handler_0:
	pushq $32	// int_no
	jmp GlobalHandler

.global IRQ_Handler_1
.type IRQ_Handler_1, @function
IRQ_Handler_1:
	pushq $33	// int_no
	jmp GlobalHandler

.global IRQ_Handler_2
.type IRQ_Handler_2, @function
IRQ_Handler_2:
	pushq $34	// int_no
	jmp GlobalHandler

.global IRQ_Handler_3
.type IRQ_Handler_3, @function
IRQ_Handler_3:
	pushq $35	// int_no
	jmp GlobalHandler

.global IRQ_Handler_4
.type IRQ_Handler_4, @function
IRQ_Handler_4:
	pushq $36	// int_no
	jmp GlobalHandler

.global IRQ_Handler_5
.type IRQ_Handler_5, @function
IRQ_Handler_5:
	pushq $37	// int_no
	jmp GlobalHandler

.global IRQ_Handler_6
.type IRQ_Handler_6, @function
IRQ_Handler_6:
	pushq $38	// int_no
	jmp GlobalHandler

.global IRQ_Handler_7
.type IRQ_Handler_7, @function
IRQ_Handler_7:
	pushq $39	// int_no
	jmp GlobalHandler

.global IRQ_Handler_8
.type IRQ_Handler_8, @function
IRQ_Handler_8:
	pushq $40	// int_no
	jmp GlobalHandler

.global IRQ_Handler_9
.type IRQ_Handler_9, @function
IRQ_Handler_9:
	pushq $41	// int_no
	jmp GlobalHandler

.global IRQ_Handler_10
.type IRQ_Handler_10, @function
IRQ_Handler_10:
	pushq $42	// int_no
	jmp GlobalHandler

.global IRQ_Handler_11
.type IRQ_Handler_11, @function
IRQ_Handler_11:
	pushq $43	// int_no
	jmp GlobalHandler

.global IRQ_Handler_12
.type IRQ_Handler_12, @function
IRQ_Handler_12:
	pushq $44	// int_no
	jmp GlobalHandler

.global IRQ_Handler_13
.type IRQ_Handler_13, @function
IRQ_Handler_13:
	pushq $45	// int_no
	jmp GlobalHandler

.global IRQ_Handler_14
.type IRQ_Handler_14, @function
IRQ_Handler_14:
	pushq $46	// int_no
	jmp GlobalHandler

.global IRQ_Handler_15
.type IRQ_Handler_15, @function
IRQ_Handler_15:
	pushq $47	// int_no
	jmp GlobalHandler




GlobalHandler:
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

	movq 120(%rsp), %rdi
	call nx_x64_interrupt

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

	addq $8, %rsp
	// Return to where we came from.

	iretq





