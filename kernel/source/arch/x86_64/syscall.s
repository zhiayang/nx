// syscall.s
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

.include "macros.s"

.macro save_regs
	push %rbx; xor %rbx, %rbx
	push %rbp; xor %rbp, %rbp
	push %r12; xor %r12, %r12
	push %r13; xor %r13, %r13
	push %r14; xor %r14, %r14
	push %r15; xor %r15, %r15
.endm

.macro restore_regs
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	pop %rbp
	pop %rbx

	xor %r9, %r9
	xor %r8, %r8
	xor %r10, %r10
	xor %rdx, %rdx
	xor %rsi, %rsi
	xor %rdi, %rdi
.endm

.global nx_x64_is_in_syscall
nx_x64_is_in_syscall:
	.byte 0

.macro do_syscall_jump_table

	cmpq (SyscallTableEntryCount), %rax
	jge 1f

	movb $1, nx_x64_is_in_syscall

	// setup the arguments. since the syscall entries are C functions, they expect
	// parameters in %rdi, %rsi, %rdx, %rcx, %r8, %r9
	// so we swap %rcx and %r10.
	mov %r10, %rcx

	shl $3, %rax
	call *SyscallTable(%rax)

	movb $0, nx_x64_is_in_syscall
1:
.endm




// note: on entry to either of these handlers, interrupts are disabled. this is because (and we found out the hard way)
// an irq (usually the timer) might come in after control is transferred here, but *before* we call swapgs. so, CS=0x8,
// and so swapgs_if_necessary doesn't swap. BUT %gs is still the user gs, so we crash the kernel. thus, we only enable
// interrupts after swapgs is complete.
// for the same reason, we disable interrupts before the swapgs to userspace.


.global nx_x64_syscall_entry
.type nx_x64_syscall_entry, @function
nx_x64_syscall_entry:
	// note: no need for explicit cli, because SF_MASK already handles that for us.
	swapgs

	// here, because we did not interrupt, literally nothing has changed except some segments, and rcx holds the return
	// address (which we need to preserve)

	// when the current thread is scheduled, TSS->RSP0 is set to the top of the kernel stack. we will use
	// this as the stack pointer. see scheduler.h for the offset for TSSBase in the CPULocalState. it is
	// currently 0x10.

	// but first, save the user stack into CPULocalState->tmpUserRsp, offset 0x38.
	movq %rsp, %gs:0x38

	movq %gs:0x10, %rsp
	movq 4(%rsp), %rsp

	// we are now on a safe stack, and can push.
	pushq %gs:0x38  // user stack
	push %r11       // user flags
	push %rcx       // user rip


	// clear it.
	movq $0, %gs:0x38

	// ok great we can do stack stuff now.
	save_regs
	sti

	do_syscall_jump_table

	// we need to handle the return value here. the "convention" i think is to
	// let the kernel return -ERRNO, and return -1; so we need to negate the return
	// value, set the userspace errno (if we are going to have that concept?), and change
	// the return value to -1?

	cli
	restore_regs

	// restore the things in prep for sysret.
	pop %rcx
	pop %r11
	pop %rsp


	swapgs
	sysretq







.global nx_x64_syscall_intr_entry
.type nx_x64_syscall_intr_entry, @function
nx_x64_syscall_intr_entry:
	swapgs_if_necessary

	save_regs
	sti

	do_syscall_jump_table

	cli
	restore_regs

	swapgs_if_necessary
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

*/










