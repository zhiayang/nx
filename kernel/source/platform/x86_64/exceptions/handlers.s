// handlers.s
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

// Defines nice ISRs.

.section .text

.global nx_x64_loadidt
.type nx_x64_loadidt, @function
nx_x64_loadidt:
	lidt (%rdi)
	ret



.global EXC_Handler_0
.type EXC_Handler_0, @function
EXC_Handler_0:
	pushq $0	// err_code
	pushq $0	// int_no
	jmp GlobalHandler

.global EXC_Handler_1
.type EXC_Handler_1, @function
EXC_Handler_1:
	pushq $0	// err_code
	pushq $1	// int_no
	jmp GlobalHandler

.global EXC_Handler_2
.type EXC_Handler_2, @function
EXC_Handler_2:
	pushq $0	// err_code
	pushq $2	// int_no
	jmp GlobalHandler

.global EXC_Handler_3
.type EXC_Handler_3, @function
EXC_Handler_3:
	pushq $0	// err_code
	pushq $3	// int_no
	jmp GlobalHandler

.global EXC_Handler_4
.type EXC_Handler_4, @function
EXC_Handler_4:
	pushq $0	// err_code
	pushq $4	// int_no
	jmp GlobalHandler

.global EXC_Handler_5
.type EXC_Handler_5, @function
EXC_Handler_5:
	pushq $0	// err_code
	pushq $5	// int_no
	jmp GlobalHandler

.global EXC_Handler_6
.type EXC_Handler_6, @function
EXC_Handler_6:
	pushq $0	// err_code
	pushq $6	// int_no
	jmp GlobalHandler

.global EXC_Handler_7
.type EXC_Handler_7, @function
EXC_Handler_7:
	pushq $0	// err_code
	pushq $7	// int_no
	jmp GlobalHandler

.global EXC_Handler_8
.type EXC_Handler_8, @function
EXC_Handler_8:
				// err_code pushed by CPU
	pushq $8	// int_no
	jmp GlobalHandler

.global EXC_Handler_9
.type EXC_Handler_9, @function
EXC_Handler_9:
	pushq $0	// err_code
	pushq $9	// int_no
	jmp GlobalHandler

.global EXC_Handler_10
.type EXC_Handler_10, @function
EXC_Handler_10:
				// err_code pushed by CPU
	pushq $10	// int_no
	jmp GlobalHandler

.global EXC_Handler_11
.type EXC_Handler_11, @function
EXC_Handler_11:
				// err_code pushed by CPU
	pushq $11	// int_no
	jmp GlobalHandler

.global EXC_Handler_12
.type EXC_Handler_12, @function
EXC_Handler_12:
				// err_code pushed by CPU
	pushq $12	// int_no
	jmp GlobalHandler

.global EXC_Handler_13
.type EXC_Handler_13, @function
EXC_Handler_13:
				// err_code pushed by CPU
	pushq $13	// int_no
	jmp GlobalHandler

.global EXC_Handler_14
.type EXC_Handler_14, @function
EXC_Handler_14:
				// err_code pushed by CPU
	pushq $14	// int_no
	jmp GlobalHandler

.global EXC_Handler_15
.type EXC_Handler_15, @function
EXC_Handler_15:
	pushq $0	// err_code
	pushq $15	// int_no
	jmp GlobalHandler

.global EXC_Handler_16
.type EXC_Handler_16, @function
EXC_Handler_16:
	pushq $0	// err_code
	pushq $16	// int_no
	jmp GlobalHandler

.global EXC_Handler_17
.type EXC_Handler_17, @function
EXC_Handler_17:
	pushq $0	// err_code
	pushq $17	// int_no
	jmp GlobalHandler

.global EXC_Handler_18
.type EXC_Handler_18, @function
EXC_Handler_18:
	pushq $0	// err_code
	pushq $18	// int_no
	jmp GlobalHandler

.global EXC_Handler_19
.type EXC_Handler_19, @function
EXC_Handler_19:
	pushq $0	// err_code
	pushq $19	// int_no
	jmp GlobalHandler

.global EXC_Handler_20
.type EXC_Handler_20, @function
EXC_Handler_20:
	pushq $0	// err_code
	pushq $20	// int_no
	jmp GlobalHandler

.global EXC_Handler_21
.type EXC_Handler_21, @function
EXC_Handler_21:
	pushq $0	// err_code
	pushq $21	// int_no
	jmp GlobalHandler

.global EXC_Handler_22
.type EXC_Handler_22, @function
EXC_Handler_22:
	pushq $0	// err_code
	pushq $22	// int_no
	jmp GlobalHandler

.global EXC_Handler_23
.type EXC_Handler_23, @function
EXC_Handler_23:
	pushq $0	// err_code
	pushq $23	// int_no
	jmp GlobalHandler

.global EXC_Handler_24
.type EXC_Handler_24, @function
EXC_Handler_24:
	pushq $0	// err_code
	pushq $24	// int_no
	jmp GlobalHandler

.global EXC_Handler_25
.type EXC_Handler_25, @function
EXC_Handler_25:
	pushq $0	// err_code
	pushq $25	// int_no
	jmp GlobalHandler

.global EXC_Handler_26
.type EXC_Handler_26, @function
EXC_Handler_26:
	pushq $0	// err_code
	pushq $26	// int_no
	jmp GlobalHandler

.global EXC_Handler_27
.type EXC_Handler_27, @function
EXC_Handler_27:
	pushq $0	// err_code
	pushq $27	// int_no
	jmp GlobalHandler

.global EXC_Handler_28
.type EXC_Handler_28, @function
EXC_Handler_28:
	pushq $0	// err_code
	pushq $28	// int_no
	jmp GlobalHandler

.global EXC_Handler_29
.type EXC_Handler_29, @function
EXC_Handler_29:
	pushq $0	// err_code
	pushq $29	// int_no
	jmp GlobalHandler

.global EXC_Handler_30
.type EXC_Handler_30, @function
EXC_Handler_30:
	pushq $0	// err_code
	pushq $30	// int_no
	jmp GlobalHandler

.global EXC_Handler_31
.type EXC_Handler_31, @function
EXC_Handler_31:
	pushq $0	// err_code
	pushq $31	// int_no
	jmp GlobalHandler




GlobalHandler:
	xchg %bx, %bx
	pushq %r15
	pushq %r14
	pushq %r13
	pushq %r12
	pushq %r11
	pushq %r10
	pushq %r9
	pushq %r8
	pushq %rdx
	pushq %rcx
	pushq %rbx
	pushq %rax
	pushq %rbp
	pushq %rsi
	pushq %rdi
	pushq %rsp


	// Push CR2 in case of page faults
	movq %cr2, %rbp
	pushq %rbp

	// pass the stack pointer as an argument, aka pointer to structure.
	movq %rsp, %rdi

	call nx_x64_exception

	addq $8, %rsp	// remove cr2
	addq $8, %rsp	// Don't pop %rsp, may not be defined.

	popq %rdi
	popq %rsi
	popq %rbp
	popq %rax
	popq %rbx
	popq %rcx
	popq %rdx
	popq %r8
	popq %r9
	popq %r10
	popq %r11
	popq %r12
	popq %r13
	popq %r14
	popq %r15

	// Remove int_no and err_code
	addq $16, %rsp

	// Return to where we came from.
	iretq




