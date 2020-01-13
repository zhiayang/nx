// functions.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace syscall
{
	extern "C" void nx_x64_syscall_entry();
	extern "C" void nx_x64_syscall_intr_entry();

	extern "C" void debug_char(char c)
	{
		println("char: %c", c);
	}

	extern "C" void debug_ptr(void* p)
	{
		println("ptr: %p", p);
	}

	extern "C" void debug_str(char* s)
	{
		println("str: %s", s);
	}

	extern "C" {
		void* SyscallTable[_SYSCALL_MAX];
		size_t SyscallTableEntryCount = sizeof(SyscallTable) / sizeof(void*);
	}

	static void init_vectors()
	{
		for(size_t i = 0; i < SyscallTableEntryCount; i++)
			SyscallTable[i] = (void*) sc_null;

		SyscallTable[SYSCALL_EXIT]                  = (void*) sc_exit;

		// ipc/sc_handlers.cpp
		SyscallTable[SYSCALL_IPC_SEND]              = (void*) sc_ipc_send;
		SyscallTable[SYSCALL_IPC_PEEK]              = (void*) sc_ipc_peek;
		SyscallTable[SYSCALL_IPC_POLL]              = (void*) sc_ipc_poll;
		SyscallTable[SYSCALL_IPC_DISCARD]           = (void*) sc_ipc_discard;
		SyscallTable[SYSCALL_IPC_RECEIVE]           = (void*) sc_ipc_receive;
		SyscallTable[SYSCALL_IPC_RECEIVE_BLOCK]     = (void*) 0;
		SyscallTable[SYSCALL_IPC_SET_SIG_HANDLER]   = (void*) sc_ipc_set_signal_handler;

		// syscall/sc_mmap.cpp
		SyscallTable[SYSCALL_MMAP_ANON]             = (void*) sc_mmap_anon;
		SyscallTable[SYSCALL_MMAP_FILE]             = (void*) sc_mmap_file;

		// syscall/sc_vfs.cpp
		SyscallTable[SYSCALL_VFS_READ]              = (void*) sc_vfs_read;
		SyscallTable[SYSCALL_VFS_WRITE]             = (void*) sc_vfs_write;

		SyscallTable[SYSCALL_GET_NANOSECOND_TS]     = (void*) sc_nanosecond_timestamp;
		SyscallTable[SYSCALL_USER_SIGNAL_LEAVE]     = (void*) sc_user_signal_leave;
		SyscallTable[SYSCALL_LOG]                   = (void*) sc_log;

		// SyscallTable[11] = (void*) debug_char;
		// SyscallTable[12] = (void*) debug_ptr;
		// SyscallTable[13] = (void*) debug_str;
	}

	void init()
	{
		init_vectors();

		if constexpr (getArchitecture() == Architecture::x64)
		{
			// install the interrupt handler:
			cpu::idt::setEntry(NX_SYSCALL_INTERRUPT_VECTOR, (addr_t) nx_x64_syscall_intr_entry, 0x08,
				/* ring3: */ true, /* nestable: */ false);

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

			// 4. mask some stuff, like the direction bit. note we mask IF, so we can enable it when we're ready.
			cpu::writeMSR(cpu::MSR_SF_MASK, cpu::FLAG_DIRECTION | cpu::FLAG_INTERRUPT);
		}
		else
		{
			abort("arch not supported!");
		}
	}


	// we have no protections in place; for now, just a memcpy.
	// TODO: data protections!!!!!!!!!!!!!!!!
	bool copy_from_user(const void* user, void* kernel, size_t len)
	{
		memcpy(kernel, user, len);
		return true;
	}

	bool copy_to_user(const void* kernel, void* user, size_t len)
	{
		memcpy(user, kernel, len);
		return true;
	}
}
}




























