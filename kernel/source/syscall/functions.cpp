// functions.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace syscall
{
	extern "C" void nx_x64_syscall_entry();
	extern "C" void nx_x64_syscall_intr_entry();

	extern "C" {
		void* SyscallTable[_SYSCALL_MAX];
		size_t SyscallTableEntryCount = sizeof(SyscallTable) / sizeof(void*);
	}

	extern "C" void tmp_debug(uint64_t val)
	{
		error("DEBUG", "*** VAL = {}", val);
	}

	static void init_vectors()
	{
		for(size_t i = 0; i < SyscallTableEntryCount; i++)
			SyscallTable[i] = (void*) do_nothing;

		SyscallTable[SYSCALL_EXIT]                  = (void*) exit;

		// syscall/sc_ipc.cpp
		SyscallTable[SYSCALL_IPC_SEND]              = (void*) ipc_send;
		SyscallTable[SYSCALL_IPC_PEEK]              = (void*) ipc_peek;
		SyscallTable[SYSCALL_IPC_POLL]              = (void*) ipc_poll;
		SyscallTable[SYSCALL_IPC_DISCARD]           = (void*) ipc_discard;
		SyscallTable[SYSCALL_IPC_RECEIVE]           = (void*) ipc_receive;
		SyscallTable[SYSCALL_IPC_RECEIVE_BLOCK]     = (void*) do_nothing;
		SyscallTable[SYSCALL_IPC_SET_SIG_HANDLER]   = (void*) ipc_set_signal_handler;
		SyscallTable[SYSCALL_IPC_SIGNAL]            = (void*) ipc_signal;
		SyscallTable[SYSCALL_IPC_SIGNAL_BLOCK]      = (void*) ipc_signal_block;
		SyscallTable[SYSCALL_IPC_FIND_SELECTOR]     = (void*) ipc_find_selector;

		// syscall/sc_rpc.cpp
		SyscallTable[SYSCALL_RPC_CALL]              = (void*) rpc_call;
		SyscallTable[SYSCALL_RPC_CALL_PROCEDURE]    = (void*) rpc_call_procedure;
		SyscallTable[SYSCALL_RPC_RETURN]            = (void*) rpc_return;
		SyscallTable[SYSCALL_RPC_WAIT_CALL]         = (void*) rpc_wait_call;
		SyscallTable[SYSCALL_RPC_WAIT_ANY_CALL]     = (void*) rpc_wait_any_call;
		SyscallTable[SYSCALL_RPC_OPEN]              = (void*) rpc_open;
		SyscallTable[SYSCALL_RPC_CLOSE]             = (void*) rpc_close;
		SyscallTable[SYSCALL_RPC_FORWARD]           = (void*) do_nothing;

		// syscall/sc_mmap.cpp
		SyscallTable[SYSCALL_MMAP_ANON]             = (void*) mmap_anon;
		SyscallTable[SYSCALL_MMAP_FILE]             = (void*) mmap_file;

		// syscall/sc_memticket.cpp
		SyscallTable[SYSCALL_MEMTICKET_CREATE]      = (void*) create_memory_ticket;
		SyscallTable[SYSCALL_MEMTICKET_COLLECT]     = (void*) collect_memory_ticket;
		SyscallTable[SYSCALL_MEMTICKET_RELEASE]     = (void*) release_memory_ticket;
		SyscallTable[SYSCALL_MEMTICKET_FIND]        = (void*) find_existing_memory_ticket;

		// syscall/sc_vfs.cpp
		SyscallTable[SYSCALL_VFS_READ]              = (void*) vfs_read;
		SyscallTable[SYSCALL_VFS_WRITE]             = (void*) vfs_write;

		SyscallTable[77]                            = (void*) tmp_debug;

		SyscallTable[SYSCALL_GET_NANOSECOND_TS]     = (void*) nanosecond_timestamp;
		SyscallTable[SYSCALL_USER_SIGNAL_LEAVE]     = (void*) user_signal_leave;
		SyscallTable[SYSCALL_LOG]                   = (void*) kernel_log;

		// SyscallTable[11] = (void*) debug_char;
		// SyscallTable[12] = (void*) debug_ptr;
		// SyscallTable[13] = (void*) debug_str;
	}

	void init()
	{
		init_vectors();

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


	// we have no protections in place; for now, just a memcpy.
	// TODO: data protections!!!!!!!!!!!!!!!!
	bool copy_to_kernel(void* kernel, const void* user, size_t len)
	{
		memcpy(kernel, user, len);
		return true;
	}

	bool copy_to_user(void* user, const void* kernel, size_t len)
	{
		memcpy(user, kernel, len);
		return true;
	}
}
}




























