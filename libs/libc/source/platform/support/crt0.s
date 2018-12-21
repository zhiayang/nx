// crt0.s
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

.section .text

.global _start
_start:

	// Set up end of the stack frame linked list.
	movq $0, %rbp
	pushq %rbp
	pushq %rbp
	movq %rsp, %rbp

	// We need those in a moment when we call main.
	pushq %rsi
	pushq %rdi

	// xchg %bx, %bx
	// Prepare signals, memory allocation, stdio and such.
	call init_libc

	// Run the global constructors.
	call _init

	// Restore argc and argv.
	popq %rdi
	popq %rsi

	// Run main
	call main

	// Terminate the process with the exit code.
	mov %rax, %rdi
	call exit





