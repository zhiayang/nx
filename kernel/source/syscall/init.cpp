// funcs.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace syscall
{
	extern "C" void nx_x64_syscall_entry();
	extern "C" void nx_x64_syscall_intr_entry();

	extern "C" int64_t debug_char(char c)
	{
		println("char: %c", c);
		return 0;
	}

	extern "C" int64_t debug_ptr(void* p)
	{
		println("ptr: %p", p);
		return 0;
	}


	extern "C" constexpr void* SyscallTable[] = {

		[SYSCALL_EXIT]      = (void*) sc_exit,      // defined in syscall/misc.cpp
		[SYSCALL_IPC_SEND]  = (void*) sc_ipc_send,  // defined in ipc/dispatcher.cpp

		[2] = (void*) debug_char,
		[3] = (void*) debug_ptr
	};

	extern "C" constexpr size_t SyscallTableEntryCount = sizeof(SyscallTable) / sizeof(void*);




	void init()
	{
		if constexpr (getArchitecture() == Architecture::x64)
		{
			// install the interrupt handler:
			cpu::idt::setEntry(NX_SYSCALL_INTERRUPT_VECTOR, (addr_t) nx_x64_syscall_intr_entry, 0x08,
				/* ring3: */ true, /* nestable: */ true);

			// install the syscall thingy.
			// 1. enable the syscall instruction. EFER.SCE is bit 0.
			cpu::writeMSR(cpu::MSR_EFER, cpu::readMSR(cpu::MSR_EFER) | 0x1);

			/*
				2. set the segments in the STAR msr. amd manual has this to say:

				The STAR register has the following fields (unless otherwise noted, all bits are read/write):

				- SYSRET CS and SS Selectors—Bits 63:48. This field is used to specify both the CS and SS
				selectors loaded into CS and SS during SYSRET. If SYSRET is returning to 64-bit mode, the CS selector is
				set to this field + 16. SS.Sel is set to this field + 8, regardless of the target mode.
				Because SYSRET always returns to CPL 3, the RPL bits 49:48 should be initialized to 11b.

				- SYSCALL CS and SS Selectors—Bits 47:32. This field is used to specify both the CS and SS
				selectors loaded into CS and SS during SYSCALL. This field is copied directly into CS.Sel.
				SS.Sel is set to this field + 8. Because SYSCALL always switches to CPL 0, the RPL bits
				33:32 should be initialized to 00b.

				our GDT is setup like this:

				0x00    null descriptor
				0x08    ring 0 code
				0x10    ring 0 data
				0x18    null descriptor    <-- this is dumb. AMD saved this space for a 32-bit code segment.
				0x20    ring 3 data
				0x28    ring 3 code

				we set syscall selector = 0x08, so we get CS=0x08, SS=0x10
				we set sysret selector = 0x18, so we get CS=0x28, SS=0x20
			*/

			uint64_t star = 0;
			star |= (0x08ULL | 0x0) << 32;      // syscall selector
			star |= (0x18ULL | 0x3) << 48;      // sysret selector

			cpu::writeMSR(cpu::MSR_STAR, star);

			// 3. lstar stores the syscall entry point.
			cpu::writeMSR(cpu::MSR_LSTAR, (uint64_t) nx_x64_syscall_entry);

			// 4. mask some stuff, like the direction bit. note we don't mask IF cos syscalls should be preemptible.
			cpu::writeMSR(cpu::MSR_SF_MASK, 0x400);
		}
		else
		{
			abort("arch not supported!");
		}
	}
}
}




























