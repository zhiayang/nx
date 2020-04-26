// switch.s
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

.include "macros.s"

.section .text

.global idle_worker
.type idle_worker, @function
idle_worker:
	jmp idle_worker

.global nx_x64_switch_to_thread
.type nx_x64_switch_to_thread, @function

.global nx_x64_tick_handler
.type nx_x64_tick_handler, @function

.global nx_x64_yield_thread
.type nx_x64_yield_thread, @function


// parameters: new rsp (%rdi), new cr3 (%rsi), newthread-sse-state (%rdx)
nx_x64_switch_to_thread:
	test %rsi, %rsi
	jz restore_state

change_cr3:
	mov %rsi, %cr3

restore_state:
	mov %rdi, %rsp

	// we can only perform surgery on the stack *after* changing cr3, since the kernel
	// stack lives in the userproc's address space and not in the kernel addrspace.
	// we passed the newthr pointer in %rcx.

	// save rdx, cos we still need it for the fpu restore.
	// the rest can be trashed, no worries.
	pushq %rdx
	mov %rcx, %rdi

	// save the stack in rbx, and align it to call the C functions
	align_stack %rbx

	call nx_x64_setup_signalled_stack

	// restore it
	unalign_stack %rbx

	// restore the saved rdx.
	popq %rdx

	mov %rdx, %rdi

	// and again:
	// save the stack in rbx, and align it to call the C functions
	align_stack %rbx

	call _ZN2nx3cpu3fpu7restoreEm

	// restore it
	unalign_stack %rbx


	pop_all_regs

	swapgs_if_necessary
	iretq




// wire this to whatever IRQ we're using to tick the kernel (be it PIT or HPET or whatever)
nx_x64_tick_handler:
	swapgs_if_necessary
	cld

	// we might be calling this at a high frequency, so for the fast path we don't save all
	// the registers -- only the caller-saved ones. according to system v abi, these are:
	// rax, rcx, rdx, rdi, rsi, r8, r9, r10, r11
	// the order doesn't really matter, but we'll just follow the standard order we use and leave holes.
	push_scratch_regs

	// align the stack using rbx (and save rbx)
	align_stack_pushreg %rbx

	call nx_x64_scheduler_tick

	// if it returned 1, we need to context switch. if 0, we iret as usual.
	// note: the scheduler_tick function will send the EOI, since we don't know (from here)
	// which IRQ number we're using for the tick.

	cmp $0, %rax
	je do_nothing

do_something:
	// need to context switch -- so we need to push all the registers.
	// but restore the stack pointer first:
	unalign_stack_popreg %rbx

	// then again:
	pop_scratch_regs
	push_all_regs

	// note: we need to pass the context via the stack pointer.
	movq %rsp, %rdi

	// we need to align the stack *again*. we don't need to additionally save rbx here, since
	// we already pushed all registers. *BUT*, we force it to be 8-byte offset. this is because
	// we use 'jmp' instead of 'call' here.
	align_stack %rbx
	subq $8, %rsp

	// scheduler code that finds the next thread to run, and calls nx_x64_switch_to_thread (above)
	// note: since this will result in an iretq, we do not have to handle its return or whatever.
	jmp nx_x64_find_and_switch_thread

	// unreachable!!!
	ud2


do_nothing:
	// restore the stack pointer, and rbx
	unalign_stack_popreg %rbx

	pop_scratch_regs

	// bye bye
	swapgs_if_necessary
	iretq





// this will be a software interrupt.
nx_x64_yield_thread:
	swapgs_if_necessary
	cld

	push_all_regs
	movq %rsp, %rdi

	// same thing above to align the stack, but with 'jmp'
	align_stack %rbx
	subq $8, %rsp

	// we just find the next thing, no issue.
	jmp nx_x64_find_and_switch_thread

	// find_and_switch will iret for us.
	ud2





















