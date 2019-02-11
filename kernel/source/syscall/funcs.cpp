// funcs.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace syscall
{
	extern "C" void nx_syscall_entry();
	extern "C" void nx_syscall_intr_entry();

	using SyscallFn_t = int64_t (*)(void*, void*, void*, void*, void*, void*);

	extern "C" int64_t debug_char(char c)
	{
		println("char: %c", c);
		return 0;
	}

	extern "C" constexpr SyscallFn_t SyscallTable[] = {
		[0] = (SyscallFn_t) debug_char,
		[1] = (SyscallFn_t) debug_char
	};

	extern "C" constexpr size_t SyscallTableEntryCount = sizeof(SyscallTable) / sizeof(SyscallFn_t);







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




























