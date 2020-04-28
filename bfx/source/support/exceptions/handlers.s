// handlers.s
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


.section .text


.global bfx_x64_loadidt
.type bfx_x64_loadidt, @function
bfx_x64_loadidt:
	lidt (%rdi)
	ret

.global bfx_x64_exception_handler_0
.type bfx_x64_exception_handler_0, @function
bfx_x64_exception_handler_0:
	pushq $0	// err_code
	pushq $0	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_1
.type bfx_x64_exception_handler_1, @function
bfx_x64_exception_handler_1:
	pushq $0	// err_code
	pushq $1	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_2
.type bfx_x64_exception_handler_2, @function
bfx_x64_exception_handler_2:
	pushq $0	// err_code
	pushq $2	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_3
.type bfx_x64_exception_handler_3, @function
bfx_x64_exception_handler_3:
	pushq $0	// err_code
	pushq $3	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_4
.type bfx_x64_exception_handler_4, @function
bfx_x64_exception_handler_4:
	pushq $0	// err_code
	pushq $4	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_5
.type bfx_x64_exception_handler_5, @function
bfx_x64_exception_handler_5:
	pushq $0	// err_code
	pushq $5	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_6
.type bfx_x64_exception_handler_6, @function
bfx_x64_exception_handler_6:
	pushq $0	// err_code
	pushq $6	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_7
.type bfx_x64_exception_handler_7, @function
bfx_x64_exception_handler_7:
	pushq $0	// err_code
	pushq $7	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_8
.type bfx_x64_exception_handler_8, @function
bfx_x64_exception_handler_8:
				// err_code pushed by CPU
	pushq $8	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_9
.type bfx_x64_exception_handler_9, @function
bfx_x64_exception_handler_9:
	pushq $0	// err_code
	pushq $9	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_10
.type bfx_x64_exception_handler_10, @function
bfx_x64_exception_handler_10:
				// err_code pushed by CPU
	pushq $10	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_11
.type bfx_x64_exception_handler_11, @function
bfx_x64_exception_handler_11:
				// err_code pushed by CPU
	pushq $11	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_12
.type bfx_x64_exception_handler_12, @function
bfx_x64_exception_handler_12:
				// err_code pushed by CPU
	pushq $12	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_13
.type bfx_x64_exception_handler_13, @function
bfx_x64_exception_handler_13:
				// err_code pushed by CPU
	pushq $13	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_14
.type bfx_x64_exception_handler_14, @function
bfx_x64_exception_handler_14:
				// err_code pushed by CPU
	pushq $14	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_15
.type bfx_x64_exception_handler_15, @function
bfx_x64_exception_handler_15:
	pushq $0	// err_code
	pushq $15	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_16
.type bfx_x64_exception_handler_16, @function
bfx_x64_exception_handler_16:
	pushq $0	// err_code
	pushq $16	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_17
.type bfx_x64_exception_handler_17, @function
bfx_x64_exception_handler_17:
	pushq $0	// err_code
	pushq $17	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_18
.type bfx_x64_exception_handler_18, @function
bfx_x64_exception_handler_18:
	pushq $0	// err_code
	pushq $18	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_19
.type bfx_x64_exception_handler_19, @function
bfx_x64_exception_handler_19:
	pushq $0	// err_code
	pushq $19	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_20
.type bfx_x64_exception_handler_20, @function
bfx_x64_exception_handler_20:
	pushq $0	// err_code
	pushq $20	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_21
.type bfx_x64_exception_handler_21, @function
bfx_x64_exception_handler_21:
	pushq $0	// err_code
	pushq $21	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_22
.type bfx_x64_exception_handler_22, @function
bfx_x64_exception_handler_22:
	pushq $0	// err_code
	pushq $22	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_23
.type bfx_x64_exception_handler_23, @function
bfx_x64_exception_handler_23:
	pushq $0	// err_code
	pushq $23	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_24
.type bfx_x64_exception_handler_24, @function
bfx_x64_exception_handler_24:
	pushq $0	// err_code
	pushq $24	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_25
.type bfx_x64_exception_handler_25, @function
bfx_x64_exception_handler_25:
	pushq $0	// err_code
	pushq $25	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_26
.type bfx_x64_exception_handler_26, @function
bfx_x64_exception_handler_26:
	pushq $0	// err_code
	pushq $26	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_27
.type bfx_x64_exception_handler_27, @function
bfx_x64_exception_handler_27:
	pushq $0	// err_code
	pushq $27	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_28
.type bfx_x64_exception_handler_28, @function
bfx_x64_exception_handler_28:
	pushq $0	// err_code
	pushq $28	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_29
.type bfx_x64_exception_handler_29, @function
bfx_x64_exception_handler_29:
	pushq $0	// err_code
	pushq $29	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_30
.type bfx_x64_exception_handler_30, @function
bfx_x64_exception_handler_30:
	pushq $0	// err_code
	pushq $30	// int_no
	jmp GlobalHandler

.global bfx_x64_exception_handler_31
.type bfx_x64_exception_handler_31, @function
bfx_x64_exception_handler_31:
	pushq $0	// err_code
	pushq $31	// int_no
	jmp GlobalHandler




GlobalHandler:
	push_all_regs
	pushq %rsp


	// Push CR2 in case of page faults
	movq %cr2, %rbp
	pushq %rbp

	// pass the stack pointer as an argument, aka pointer to structure.
	movq %rsp, %rdi

	call bfx_x64_handle_exception

	addq $8, %rsp	// remove cr2
	addq $8, %rsp	// Don't pop %rsp, may not be defined.

	pop_all_regs

	// Remove int_no and err_code
	addq $16, %rsp

	iretq














