// syscall.s
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

.macro save_regs
	push %rbx
	push %rbp
	push %r12
	push %r13
	push %r14
	push %r15
.endm

.macro restore_regs
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	pop %rbp
	pop %rbx
.endm


.extern SyscallTable

.global nx_syscall_entry
.type nx_syscall_entry, @function
nx_syscall_entry:
	swapgs

	// here, because we did not interrupt, literally nothing has changed except some segments, and rcx holds the return
	// address (which we need to preserve)

	// step 1: store the user IP and SP. see scheduler.h: the field is at offset 48.
	movq %rcx, %gs:0x38
	movq %rsp, %gs:0x40



	swapgs
	sysret







.global nx_syscall_intr_entry
.type nx_syscall_intr_entry, @function
nx_syscall_intr_entry:
	swapgs
	save_regs

	cmp $SyscallTableEntryCount, %rax
	jl fail

	// %rax contains the syscall number; shift it right by 3 to get the index
	// into the table.
	shl $3, %rax

	// setup the arguments. since the syscall entries are C functions, they expect
	// parameters in %rdi, %rsi, %rdx, %rcx, %r8, %r9
	// so we swap %rcx and %r10.
	xchg %rcx, %r10

	call *SyscallTable(%rax)

fail:
	restore_regs
	swapgs
	iretq










/*
	* system call calling convention *
	1. software interrupt interface

	syscall vector in %rax, up to 6 arguments: %rdi, %rsi, %rdx, %r10, %r8, %r9. as per sysv ABI, but replacing %rcx with %r10.
	caller-saved registers (%rax, %rcx, %rdx, %rdi, %rsi, %r8, %r9, %r10, %r11) will be clobbered.

	interrupt vector is 0xF8. return value is in %rax.


	2. syscall/sysret interface

	vector and arguments identical to the interrupt interface.
	caller-saved registers (%rax, %rcx, %rdx, %rdi, %rsi, %r8, %r9, %r10, %r11) will be clobbered.



	* system call vector list *

	0       int exit(int code)
	1       size_t read(fd_t fd, void* buf, size_t count)
	2       size_t write(fd_t fd, void* buf, size_t count)
*/










