// user_kernel_stubs.s
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.


.align 0x1000
.global nx_user_kernel_stubs_begin
nx_user_kernel_stubs_begin:
	.byte 0


// as described in signal.cpp, this function takes the address of the signal
// handler in %r9, directly forwards its arguments (which are already in %rdi, %rsi, %rdx, etc.)
// and calls %r9.
.global nx_x64_user_signal_enter
.type nx_x64_user_signal_enter, @function
nx_x64_user_signal_enter:
	// before we can do anything, we need to adjust the stack.
	// when we are thrown here from the scheduler, RSP0 always points to the bottom of the user stack.
	// so, subtract 128 for the red zone:
	sub $128, %rsp



	test %r9, %r9
	jz 1f

	call *%r9

1:
	// this is hardcoded, but it's fine... i guess.
	// SYSCALL_USER_SIGNAL_LEAVE == 98
	movq $98, %rax
	syscall

	// we shouldn't get here!!
	ud2

.global nx_user_kernel_stubs_end
nx_user_kernel_stubs_end:
	.byte 0
