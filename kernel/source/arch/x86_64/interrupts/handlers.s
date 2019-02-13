// Interrupts.s
// Copyright (c) 2013 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

.include "macros.s"


.section .text

.global nx_x64_irq_handler_0
.type nx_x64_irq_handler_0, @function
nx_x64_irq_handler_0:
	pushq $0	// int_no
	jmp GlobalHandler

.global nx_x64_irq_handler_1
.type nx_x64_irq_handler_1, @function
nx_x64_irq_handler_1:
	pushq $1	// int_no
	jmp GlobalHandler

.global nx_x64_irq_handler_2
.type nx_x64_irq_handler_2, @function
nx_x64_irq_handler_2:
	pushq $2	// int_no
	jmp GlobalHandler

.global nx_x64_irq_handler_3
.type nx_x64_irq_handler_3, @function
nx_x64_irq_handler_3:
	pushq $3	// int_no
	jmp GlobalHandler

.global nx_x64_irq_handler_4
.type nx_x64_irq_handler_4, @function
nx_x64_irq_handler_4:
	pushq $4	// int_no
	jmp GlobalHandler

.global nx_x64_irq_handler_5
.type nx_x64_irq_handler_5, @function
nx_x64_irq_handler_5:
	pushq $5	// int_no
	jmp GlobalHandler

.global nx_x64_irq_handler_6
.type nx_x64_irq_handler_6, @function
nx_x64_irq_handler_6:
	pushq $6	// int_no
	jmp GlobalHandler

.global nx_x64_irq_handler_7
.type nx_x64_irq_handler_7, @function
nx_x64_irq_handler_7:
	pushq $7	// int_no
	jmp GlobalHandler

.global nx_x64_irq_handler_8
.type nx_x64_irq_handler_8, @function
nx_x64_irq_handler_8:
	pushq $8	// int_no
	jmp GlobalHandler

.global nx_x64_irq_handler_9
.type nx_x64_irq_handler_9, @function
nx_x64_irq_handler_9:
	pushq $9	// int_no
	jmp GlobalHandler

.global nx_x64_irq_handler_10
.type nx_x64_irq_handler_10, @function
nx_x64_irq_handler_10:
	pushq $10	// int_no
	jmp GlobalHandler

.global nx_x64_irq_handler_11
.type nx_x64_irq_handler_11, @function
nx_x64_irq_handler_11:
	pushq $11	// int_no
	jmp GlobalHandler

.global nx_x64_irq_handler_12
.type nx_x64_irq_handler_12, @function
nx_x64_irq_handler_12:
	pushq $12	// int_no
	jmp GlobalHandler

.global nx_x64_irq_handler_13
.type nx_x64_irq_handler_13, @function
nx_x64_irq_handler_13:
	pushq $13	// int_no
	jmp GlobalHandler

.global nx_x64_irq_handler_14
.type nx_x64_irq_handler_14, @function
nx_x64_irq_handler_14:
	pushq $14	// int_no
	jmp GlobalHandler

.global nx_x64_irq_handler_15
.type nx_x64_irq_handler_15, @function
nx_x64_irq_handler_15:
	pushq $15	// int_no
	jmp GlobalHandler




GlobalHandler:
	push_all_regs

	movq 120(%rsp), %rdi
	call nx_x64_handle_irq

	pop_all_regs

	// remove the interrupt number
	addq $8, %rsp

	iretq





