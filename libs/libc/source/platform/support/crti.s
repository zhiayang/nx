// crti.s
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

.section .text

.global call_constructors
.type call_constructors, @function
call_constructors:
	push %rbp
	mov %rsp, %rbp

	// r12-r15 are preserved, so use those.
	movq $__init_array_start, %r12
	movq $__init_array_end, %r13

1:
	cmp %r12, %r13
	jz 2f

	mov (%r12), %r14

	callq *%r14

	addq $0x8, %r12
	jmp 1b
2:
	pop %rbp
	ret



.global call_destructors
.type call_destructors, @function
call_destructors:
	push %rbp
	mov %rsp, %rbp

	// r12-r15 are preserved, so use those.
	movq $__fini_array_start, %r12
	movq $__fini_array_end, %r13

1:
	cmp %r12, %r13
	jz 2f

	mov (%r12), %r14

	callq *%r14

	addq $0x8, %r12
	jmp 1b
2:
	pop %rbp
	ret

