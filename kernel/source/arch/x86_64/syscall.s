// syscall.s
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

.macro save_regs
	push %rbx
	push %rbp
	push %r11
	push %r12
	push %r13
	push %r14
	push %r15

	xor %rbx, %rbx
	xor %rbp, %rbp
	xor %r11, %r11
	xor %r12, %r12
	xor %r13, %r13
	xor %r14, %r14
	xor %r15, %r15
.endm

.macro restore_regs
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	pop %r11
	pop %rbp
	pop %rbx
.endm

.macro do_syscall_jump_table

	cmp $SyscallTableEntryCount, %rax
	jl 1f

	// %rax contains the syscall number; shift it right by 3 to get the index
	// into the table.
	shl $3, %rax

	// setup the arguments. since the syscall entries are C functions, they expect
	// parameters in %rdi, %rsi, %rdx, %rcx, %r8, %r9
	// so we swap %rcx and %r10.
	xchg %rcx, %r10

	call *SyscallTable(%rax)

1:
.endm




// note: on entry to either of these handlers, interrupts are disabled. this is because (and we found out the hard way)
// an irq (usually the timer) might come in after control is transferred here, but *before* we call swapgs. so, CS=0x8,
// and so swapgs_if_necessary doesn't swap. BUT %gs is still the user gs, so we crash the kernel. thus, we only enable
// interrupts after swapgs is complete.
// for the same reason, we disable interrupts before the swapgs to userspace.


.extern SyscallTable

.global nx_x64_syscall_entry
.type nx_x64_syscall_entry, @function
nx_x64_syscall_entry:
	swapgs

	// here, because we did not interrupt, literally nothing has changed except some segments, and rcx holds the return
	// address (which we need to preserve)

	// store the user IP, SP, and flags. see scheduler.h for the offsets.
	movq %rcx, %gs:0x38
	movq %rsp, %gs:0x40
	movq %r11, %gs:0x48

	// ok, now we can use %rcx as a scratch register of sorts. the next step is to get us a stack pointer.
	// when the current thread is scheduled, TSS->RSP0 is set to the top of the kernel stack. we will use
	// this as the stack pointer. see scheduler.h for the offset for TSSBase in the CPULocalState.

	// load TSSBase into RCX
	movq %gs:0x10, %rcx

	// RSP0 is at offset 4 from the TSSBase
	movq 4(%rcx), %rsp


	// interrupts r safe
	sti

	// ok great we can do stack stuff now.
	save_regs


	do_syscall_jump_table


	restore_regs

	// ok, now we need to restore the user sp and ip and flags
	movq %gs:0x38, %rcx
	movq %gs:0x40, %rsp
	movq %gs:0x48, %r11

	// and off we go
	cli
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










