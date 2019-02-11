// helper.s
// Copyright (c) 2014 - 2019, zhiayang
// Licensed under the Apache License Version 2.0.



.global nx_syscall_0; .type nx_syscall_0, @function
.global nx_syscall_1; .type nx_syscall_1, @function
.global nx_syscall_2; .type nx_syscall_2, @function
.global nx_syscall_3; .type nx_syscall_3, @function
.global nx_syscall_4; .type nx_syscall_4, @function
.global nx_syscall_5; .type nx_syscall_5, @function
.global nx_syscall_6; .type nx_syscall_6, @function

nx_syscall_0:
	int $0xF8

nx_syscall_1:
	int $0xF8

nx_syscall_2:
	int $0xF8

nx_syscall_3:
	int $0xF8

nx_syscall_4:
	int $0xF8

nx_syscall_5:
	int $0xF8

nx_syscall_6:
	int $0xF8



















































.global Syscall0Param; .type Syscall0Param, @function
.global Syscall1Param; .type Syscall1Param, @function
.global Syscall2Param; .type Syscall2Param, @function
.global Syscall3Param; .type Syscall3Param, @function
.global Syscall4Param; .type Syscall4Param, @function
.global Syscall5Param; .type Syscall5Param, @function
.global Syscall6Param; .type Syscall6Param, @function


Syscall0Param:
	mov %rdi, %r10
	push %r13
	int $0xF8

	// save rax, then get the address of errno
	push %rax
	call __fetch_errno
	mov %r13, (%rax)
	pop %rax

	pop %r13
	ret

Syscall1Param:
	mov %rsi, %r10
	push %r13
	int $0xF8

	// save rax, then get the address of errno
	push %rax
	call __fetch_errno
	mov %r13, (%rax)
	pop %rax

	pop %r13
	ret

Syscall2Param:
	mov %rdx, %r10
	push %r13
	int $0xF8

	// save rax, then get the address of errno
	push %rax
	call __fetch_errno
	mov %r13, (%rax)
	pop %rax

	pop %r13
	ret

Syscall3Param:
	mov %rcx, %r10
	push %r13
	int $0xF8

	// save rax, then get the address of errno
	push %rax
	call __fetch_errno
	mov %r13, (%rax)
	pop %rax

	pop %r13
	ret

Syscall4Param:
	mov %r8, %r10
	push %r13
	int $0xF8

	// save rax, then get the address of errno
	push %rax
	call __fetch_errno
	mov %r13, (%rax)
	pop %rax

	pop %r13
	ret

Syscall5Param:
	mov %r9, %r10
	push %r13
	int $0xF8

	// save rax, then get the address of errno
	push %rax
	call __fetch_errno
	mov %r13, (%rax)
	pop %rax

	pop %r13
	ret



