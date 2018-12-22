// helper.s
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

.global Syscall0Param
.global Syscall1Param
.global Syscall2Param
.global Syscall3Param
.global Syscall4Param
.global Syscall5Param

.type Syscall0Param, @function
.type Syscall1Param, @function
.type Syscall2Param, @function
.type Syscall3Param, @function
.type Syscall4Param, @function
.type Syscall5Param, @function

// any errno set by a syscall is stored in 0x2610 and preserved across context switches.
// so we access 0x2610 to get the errno.

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



