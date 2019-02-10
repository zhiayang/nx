// syscall.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace syscall
{
	extern "C" void nx_syscall_entry();
	extern "C" void nx_syscall_intr_entry();

	using SyscallFn_t = int64_t (*)(void*, void*, void*, void*, void*, void*);

	extern "C" constexpr SyscallFn_t SyscallTable[] = {
		[0] = (SyscallFn_t) 0
	};





	extern "C" int64_t nx_syscall_dispatch()
	{
		return 0;
	}







	void init()
	{
		if constexpr (getArchitecture() == Architecture::x64)
		{
			// install the interrupt handler:
			cpu::idt::setEntry(NX_SYSCALL_INTERRUPT_VECTOR, (addr_t) nx_syscall_intr_entry, 0x08,
				/* ring3: */ true, /* nestable: */ true);


			// install the syscall thingy.
			// TODO !! MSR stuff
		}
		else
		{
			abort("arch not supported!");
		}
	}
}
}








/*
	* system call calling convention *
	1. software interrupt interface

	syscall vector in %rax, up to 6 arguments: %rdi, %rsi, %rdx, %r10, %r8, %r9. as per sysv ABI, but replacing %rcx with %r10
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
























