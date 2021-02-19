// handlers.s
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

.include "macros.s"

.section .text





.global nx_x64_exception_handler_0
.type nx_x64_exception_handler_0, @function
nx_x64_exception_handler_0:
	pushq $0	// err_code
	pushq $0	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_1
.type nx_x64_exception_handler_1, @function
nx_x64_exception_handler_1:
	pushq $0	// err_code
	pushq $1	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_2
.type nx_x64_exception_handler_2, @function
nx_x64_exception_handler_2:
	pushq $0	// err_code
	pushq $2	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_3
.type nx_x64_exception_handler_3, @function
nx_x64_exception_handler_3:
	pushq $0	// err_code
	pushq $3	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_4
.type nx_x64_exception_handler_4, @function
nx_x64_exception_handler_4:
	pushq $0	// err_code
	pushq $4	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_5
.type nx_x64_exception_handler_5, @function
nx_x64_exception_handler_5:
	pushq $0	// err_code
	pushq $5	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_6
.type nx_x64_exception_handler_6, @function
nx_x64_exception_handler_6:
	pushq $0	// err_code
	pushq $6	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_7
.type nx_x64_exception_handler_7, @function
nx_x64_exception_handler_7:
	pushq $0	// err_code
	pushq $7	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_8
.type nx_x64_exception_handler_8, @function
nx_x64_exception_handler_8:
				// err_code pushed by CPU
	pushq $8	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_9
.type nx_x64_exception_handler_9, @function
nx_x64_exception_handler_9:
	pushq $0	// err_code
	pushq $9	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_10
.type nx_x64_exception_handler_10, @function
nx_x64_exception_handler_10:
				// err_code pushed by CPU
	pushq $10	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_11
.type nx_x64_exception_handler_11, @function
nx_x64_exception_handler_11:
				// err_code pushed by CPU
	pushq $11	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_12
.type nx_x64_exception_handler_12, @function
nx_x64_exception_handler_12:
				// err_code pushed by CPU
	pushq $12	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_13
.type nx_x64_exception_handler_13, @function
nx_x64_exception_handler_13:
				// err_code pushed by CPU
	pushq $13	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_14
.type nx_x64_exception_handler_14, @function
nx_x64_exception_handler_14:
				// err_code pushed by CPU
	pushq $14	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_15
.type nx_x64_exception_handler_15, @function
nx_x64_exception_handler_15:
	pushq $0	// err_code
	pushq $15	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_16
.type nx_x64_exception_handler_16, @function
nx_x64_exception_handler_16:
	pushq $0	// err_code
	pushq $16	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_17
.type nx_x64_exception_handler_17, @function
nx_x64_exception_handler_17:
	pushq $0	// err_code
	pushq $17	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_18
.type nx_x64_exception_handler_18, @function
nx_x64_exception_handler_18:
	pushq $0	// err_code
	pushq $18	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_19
.type nx_x64_exception_handler_19, @function
nx_x64_exception_handler_19:
	pushq $0	// err_code
	pushq $19	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_20
.type nx_x64_exception_handler_20, @function
nx_x64_exception_handler_20:
	pushq $0	// err_code
	pushq $20	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_21
.type nx_x64_exception_handler_21, @function
nx_x64_exception_handler_21:
	pushq $0	// err_code
	pushq $21	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_22
.type nx_x64_exception_handler_22, @function
nx_x64_exception_handler_22:
	pushq $0	// err_code
	pushq $22	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_23
.type nx_x64_exception_handler_23, @function
nx_x64_exception_handler_23:
	pushq $0	// err_code
	pushq $23	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_24
.type nx_x64_exception_handler_24, @function
nx_x64_exception_handler_24:
	pushq $0	// err_code
	pushq $24	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_25
.type nx_x64_exception_handler_25, @function
nx_x64_exception_handler_25:
	pushq $0	// err_code
	pushq $25	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_26
.type nx_x64_exception_handler_26, @function
nx_x64_exception_handler_26:
	pushq $0	// err_code
	pushq $26	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_27
.type nx_x64_exception_handler_27, @function
nx_x64_exception_handler_27:
	pushq $0	// err_code
	pushq $27	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_28
.type nx_x64_exception_handler_28, @function
nx_x64_exception_handler_28:
	pushq $0	// err_code
	pushq $28	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_29
.type nx_x64_exception_handler_29, @function
nx_x64_exception_handler_29:
	pushq $0	// err_code
	pushq $29	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_30
.type nx_x64_exception_handler_30, @function
nx_x64_exception_handler_30:
	pushq $0	// err_code
	pushq $30	// int_no
	jmp GlobalHandler

.global nx_x64_exception_handler_31
.type nx_x64_exception_handler_31, @function
nx_x64_exception_handler_31:
	pushq $0	// err_code
	pushq $31	// int_no
	jmp GlobalHandler


.macro exception_handler_begin
	swapgs_if_necessary 0x18

	push_all_regs
	cld

	pushq %rsp

	// Push CR2 in case of page faults
	movq %cr2, %rbp
	push %rbp

	// pass the stack pointer as an argument, aka pointer to structure.
	mov %rsp, %rdi

	call nx_x64_enter_intr_context

	// we are free to modify rbx, since it's callee-saved.
	align_stack %rbx
.endm


.macro exception_handler_end

	unalign_stack %rbx

	call nx_x64_leave_intr_context

	addq $8, %rsp	// remove cr2
	addq $8, %rsp	// remove rsp

	pop_all_regs

	// Remove int_no and err_code
	addq $16, %rsp

	// note: the offset for this swapgs is the default (0x8) because we already removed the error code and number
	// from the stack above.
	swapgs_if_necessary
.endm



.align 16
.global __last_saved_error_code
__last_saved_error_code:
.quad 0

.global nx_x64_pagefault_handler
.type nx_x64_pagefault_handler, @function
nx_x64_pagefault_handler:
	// error code pushed by cpu
	popq (__last_saved_error_code)
	pushq (__last_saved_error_code)
	pushq $14
	exception_handler_begin

	call nx_x64_handle_page_fault_internal

	exception_handler_end
	iretq








GlobalHandler:
	exception_handler_begin

	call nx_x64_handle_exception

	exception_handler_end
	iretq














