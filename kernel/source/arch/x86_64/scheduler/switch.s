// switch.s
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

.include "macros.s"

.section .text

.global nx_x64_switch_to_thread
.type nx_x64_switch_to_thread, @function

.global nx_x64_tick_handler
.type nx_x64_tick_handler, @function

.global nx_x64_yield_thread
.type nx_x64_yield_thread, @function


// note: why this is only called "if necessary":
// if were interrupted while in ring 0 (eg. while running kernel threads), then we *don't want* to swap gs,
// because gsbase will already be valid. similarly, when we are leaving *to* ring 0, we don't want to swap either.

// (we can call this macro on exit as well, but only after restoring the registers -- so the only things on the stack
// are the 5 things that the cpu pushed for the interrupt stack frame)

// on the contrary, if we came from ring 3, cs will be 0x1B, so we will do a swapgs; the same applies when leaving to ring 3.
.macro swapgs_if_necessary
	cmp $0x08, 0x8(%rsp)
	je 1f
	swapgs
1:
.endm



// parameters: new rsp (%rdi), new cr3 (%rsi), newthread-sse-state (%rdx)
nx_x64_switch_to_thread:
	test %rsi, %rsi
	jz restore_regs

change_cr3:
	mov %rsi, %cr3

restore_regs:
	mov %rdi, %rsp

	mov %rdx, %rdi
	call _ZN2nx3cpu3fpu7restoreEm

	pop_all_regs

	swapgs_if_necessary
	iretq




// wire this to whatever IRQ we're using to tick the kernel (be it PIT or HPET or whatever)
nx_x64_tick_handler:
	swapgs_if_necessary

	// we might be calling this at a high frequency, so for the fast path we don't save all
	// the registers -- only the caller-saved ones. according to system v abi, these are:
	// rax, rcx, rdx, rdi, rsi, r8, r9, r10, r11
	// the order doesn't really matter, but we'll just follow the standard order we use and leave holes.
	push_scratch_regs

	// this actually needs no arguments, it will use... *gasp*
	// G L O B A L   S T A T E
	call nx_x64_scheduler_tick

	// if it returned 1, we need to context switch. if 0, we iret as usual.
	// note: the scheduler_tick function will send the EOI, since we don't know (from here)
	// which IRQ number we're using for the tick.

	cmp $0, %rax
	je do_nothing

do_something:
	// need to context switch -- so we need to push all the registers.
	pop_scratch_regs
	push_all_regs

	// note: we need to pass the context via the stack pointer.
	movq %rsp, %rdi

	// scheduler code that finds the next thread to run, and calls nx_x64_switch_to_thread (above)
	// note: since this will result in an iretq, we do not have to handle its return or whatever.
	jmp nx_x64_find_and_switch_thread

	// unreachable!!!
	ud2


do_nothing:
	pop_scratch_regs

	// bye bye
	swapgs_if_necessary
	iretq







// this will be a software interrupt.
nx_x64_yield_thread:
	swapgs_if_necessary

	push_all_regs

	// we just find the next thing, no issue.

	movq %rsp, %rdi
	jmp nx_x64_find_and_switch_thread

	// find_and_switch will iret for us.
	ud2





















