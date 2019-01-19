// Interrupts.s
// Copyright (c) 2013 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

// Defines nice ISRs.

.section .text

.global IRQ_Handler_0
.type IRQ_Handler_0, @function
IRQ_Handler_0:
	pushq $0	// int_no
	jmp GlobalHandler

.global IRQ_Handler_1
.type IRQ_Handler_1, @function
IRQ_Handler_1:
	pushq $1	// int_no
	jmp GlobalHandler

.global IRQ_Handler_2
.type IRQ_Handler_2, @function
IRQ_Handler_2:
	pushq $2	// int_no
	jmp GlobalHandler

.global IRQ_Handler_3
.type IRQ_Handler_3, @function
IRQ_Handler_3:
	pushq $3	// int_no
	jmp GlobalHandler

.global IRQ_Handler_4
.type IRQ_Handler_4, @function
IRQ_Handler_4:
	pushq $4	// int_no
	jmp GlobalHandler

.global IRQ_Handler_5
.type IRQ_Handler_5, @function
IRQ_Handler_5:
	pushq $5	// int_no
	jmp GlobalHandler

.global IRQ_Handler_6
.type IRQ_Handler_6, @function
IRQ_Handler_6:
	pushq $6	// int_no
	jmp GlobalHandler

.global IRQ_Handler_7
.type IRQ_Handler_7, @function
IRQ_Handler_7:
	pushq $7	// int_no
	jmp GlobalHandler

.global IRQ_Handler_8
.type IRQ_Handler_8, @function
IRQ_Handler_8:
	pushq $8	// int_no
	jmp GlobalHandler

.global IRQ_Handler_9
.type IRQ_Handler_9, @function
IRQ_Handler_9:
	pushq $9	// int_no
	jmp GlobalHandler

.global IRQ_Handler_10
.type IRQ_Handler_10, @function
IRQ_Handler_10:
	pushq $10	// int_no
	jmp GlobalHandler

.global IRQ_Handler_11
.type IRQ_Handler_11, @function
IRQ_Handler_11:
	pushq $11	// int_no
	jmp GlobalHandler

.global IRQ_Handler_12
.type IRQ_Handler_12, @function
IRQ_Handler_12:
	pushq $12	// int_no
	jmp GlobalHandler

.global IRQ_Handler_13
.type IRQ_Handler_13, @function
IRQ_Handler_13:
	pushq $13	// int_no
	jmp GlobalHandler

.global IRQ_Handler_14
.type IRQ_Handler_14, @function
IRQ_Handler_14:
	pushq $14	// int_no
	jmp GlobalHandler

.global IRQ_Handler_15
.type IRQ_Handler_15, @function
IRQ_Handler_15:
	pushq $15	// int_no
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





