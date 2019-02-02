// switch.s
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

.include "macros.s"

.section .text

.global nx_x64_switch_to_thread
.type nx_x64_switch_to_thread, @function

.global nx_x64_tick_handler
.type nx_x64_tick_handler, @function


// parameters: new rsp (%rdi), new cr3 (%rsi)
nx_x64_switch_to_thread:

	// if the new cr3 is 0, then we do not switch.
	cmp $0, %rsi
	je restore_registers

change_cr3:
	mov %rsi, %cr3

restore_registers:
	mov %rdi, %rsp

	pop_all_regs

	iretq









// wire this to whatever IRQ we're using to tick the kernel (be it PIT or HPET or whatever)
nx_x64_tick_handler:
	// we might be calling this at a high frequency, so for the fast path we don't save all
	// the registers -- only the caller-saved ones. according to system v abi, these are:
	// rax, rcx, rdx, rdi, rsi, r8, r9, r10, r11
	// the order doesn't really matter, but we'll just follow the standard order we use and leave holes.
	push_scratch_regs

	// this actually needs no arguments, it will use... *gasp*
	// G L O B A L   S T A T E
	xor %rax, %rax
	call nx_x64_scheduler_tick

	// if it returned 1, we need to context switch. if 0, we iret as usual.
	// note: the scheduler_tick function will send the EOI, since we don't know (from here)
	// which IRQ number we're using for the tick.

	cmp $0, %eax
	je do_nothing

do_something:
	// need to context switch -- so we need to push all the registers.
	pop_scratch_regs
	push_all_regs

	// TODO: save/restore floating point state!!!! (fxsave, fxrstor)

	// note: we need to pass the context via the stack pointer.
	movq %rsp, %rdi

	// scheduler code that finds the next thread to run, and calls nx_x64_switch_to_thread (above)
	// note: since this will result in an iretq, we do not have to handle its return or whatever.
	call nx_x64_find_and_switch_thread

	// unreachable!!!
	ud2


do_nothing:
	pop_scratch_regs

	// bye bye
	iretq

















