// misc.s
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

// miscellaneous assembly support functions.

.include "macros.s"

.global nx_x64_clear_segments
.type nx_x64_clear_segments, @function
nx_x64_clear_segments:
	xor %ax, %ax
	mov %ax, %ds
	mov %ax, %es
	ret


.global nx_x64_serial_interrupt
.type nx_x64_serial_interrupt, @function
nx_x64_serial_interrupt:
	swapgs_if_necessary
	push_scratch_regs
	cld

	align_stack_pushreg %rbx
	call nx_x64_enter_intr_context

	call _ZN2nx6serial11irq_handlerEv

	call nx_x64_leave_intr_context
	unalign_stack_popreg %rbx


	pop_scratch_regs
	swapgs_if_necessary
	iretq






.global _ZN2nx8platform23enter_interrupt_contextEv; .type _ZN2nx8platform23enter_interrupt_contextEv, @function
.global nx_x64_enter_intr_context; .type nx_x64_enter_intr_context, @function
_ZN2nx8platform23enter_interrupt_contextEv:
nx_x64_enter_intr_context:
	lock incl %gs:0x50
	ret

.global _ZN2nx8platform23leave_interrupt_contextEv; .type _ZN2nx8platform23leave_interrupt_contextEv, @function
.global nx_x64_leave_intr_context; .type nx_x64_leave_intr_context, @function
_ZN2nx8platform23leave_interrupt_contextEv:
nx_x64_leave_intr_context:
	lock decl %gs:0x50
	ret


.global _ZN2nx8platform14is_interruptedEv; .type _ZN2nx8platform14is_interruptedEv, @function
.global nx_x64_is_in_intr_context; .type nx_x64_is_in_intr_context, @function
_ZN2nx8platform14is_interruptedEv:
nx_x64_is_in_intr_context:
	movl %gs:0x50, %eax
	test %eax, %eax
	setnz %al
	movzx %al, %rax
	ret





.global _ZN2nx16get_flags_registerEv
.type _ZN2nx16get_flags_registerEv, @function
_ZN2nx18get_flags_registerEv:
	pushfq
	pop %rax
	ret







.global _ZN2nx8platform8read_msrEj
.type _ZN2nx8platform8read_msrEj, @function
_ZN2nx8platform8read_msrEj:
	// %ecx is the msr to read (but the paramter comes in in %rdi)
	// writing to %ecx clears the top 32-bits.
	mov %edi, %ecx

	// ok, read the msr. it returns in %edx:%eax
	rdmsr

	// shift edx by 32
	shl $32, %rdx
	orq %rdx, %rax

	ret

.global _ZN2nx8platform9write_msrEjm
.type _ZN2nx8platform9write_msrEjm, @function
_ZN2nx8platform9write_msrEjm:
	// %ecx is the msr to write (but the paramter comes in in %rdi)
	// writing to %ecx clears the top 32-bits.
	mov %edi, %ecx

	// %rsi is the value to write, but wrmsr wants it in %edx:%eax
	mov %esi, %eax
	mov %rsi, %rdx
	shr $32, %rdx

	wrmsr
	ret


.global _ZN2nx8platform8read_cr0Ev
.type _ZN2nx8platform8read_cr0Ev, @function
_ZN2nx8platform8read_cr0Ev:
	movq %cr0, %rax
	ret

.global _ZN2nx8platform9write_cr0Em
.type _ZN2nx8platform9write_cr0Em, @function
_ZN2nx8platform9write_cr0Em:
	movq %rdi, %cr0
	ret

.global _ZN2nx8platform8read_cr4Ev
.type _ZN2nx8platform8read_cr4Ev, @function
_ZN2nx8platform8read_cr4Ev:
	movq %cr4, %rax
	ret

.global _ZN2nx8platform9write_cr4Em
.type _ZN2nx8platform9write_cr4Em, @function
_ZN2nx8platform9write_cr4Em:
	movq %rdi, %cr4
	ret


