// syscall.s
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.


.global nx_syscall_entry
.type nx_syscall_entry, @function
nx_syscall_entry:
	swapgs

	// here, because we did not interrupt, literally nothing has changed except some segments, and rcx holds the return
	// address (which we need to preserve)

	// step 1: store the user IP and SP. see scheduler.h: the field is at offset 48.
	movq %rcx, %gs:0x38
	movq %rsp, %gs:0x40



	call nx_syscall_dispatch

	swapgs
	sysret







.global nx_syscall_intr_entry
.type nx_syscall_intr_entry, @function
nx_syscall_intr_entry:
	swapgs

	// here, because of rsp0 in the tss, we will have a proper kernel
	// stack with which to do things. so, we can just call the dispatcher.

	call nx_syscall_dispatch

	swapgs
	iretq
