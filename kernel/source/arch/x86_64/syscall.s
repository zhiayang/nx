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

.macro do_syscall_jump_table

	cmpq (SyscallTableEntryCount), %rax
	jge 1f


	// setup the arguments. since the syscall entries are C functions, they expect
	// parameters in %rdi, %rsi, %rdx, %rcx, %r8, %r9
	// so we swap %rcx and %r10.
	mov %r10, %rcx

	shl $3, %rax
	call *SyscallTable(%rax)
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
	swapgs

	// here, because we did not interrupt, literally nothing has changed except some segments, and rcx holds the return
	// address (which we need to preserve)

	// fetch the kernel stack into r11:
	// when the current thread is scheduled, TSS->RSP0 is set to the top of the kernel stack. we will use
	// this as the stack pointer. see scheduler.h for the offset for TSSBase in the CPULocalState.
	movq %gs:0x10, %r11
	movq 4(%r11), %r11

	// make that our stack pointer. now %r11 contains the user stack
	xchg %rsp, %r11

	// we are now on the kernel stack. save the user rsp and user rip.
	pushq %r11
	pushq %rcx

	// interrupts r safe
	sti

	// ok great we can do stack stuff now.
	save_regs

	do_syscall_jump_table

	restore_regs

	// and off we go
	cli

	// note that we always set rflags to 0x202; IF set, bit 1 (reserved) set, everything else clear
	popq %rcx
	popq %rsp
	movq $0x202, %r11

	swapgs
	sysretq







.global nx_x64_syscall_intr_entry
.type nx_x64_syscall_intr_entry, @function
nx_x64_syscall_intr_entry:
	swapgs
	sti

	save_regs

	do_syscall_jump_table

	restore_regs

	cli
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

*/










