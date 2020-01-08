// ipc.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <syscall.h>
#include <nx/thread_local.h>

#include "ipc.h"

namespace nx {
namespace ipc
{
	int send(uint64_t target, uint64_t a, uint64_t b, uint64_t c, uint64_t d)
	{
		int64_t ret = 0;
		__nx_syscall_5(SYSCALL_IPC_SEND, ret, target, a, b, c, d);

		return ret;
	}

	void discard()
	{
		__nx_syscall_0v(SYSCALL_IPC_DISCARD);
	}

	uint64_t peek(uint64_t* a, uint64_t* b, uint64_t* c, uint64_t* d)
	{
		uint64_t ret = 0;
		__nx_syscall_4(SYSCALL_IPC_PEEK, ret, a, b, c, d);

		return ret;
	}

	uint64_t receive(uint64_t* a, uint64_t* b, uint64_t* c, uint64_t* d)
	{
		uint64_t ret = 0;
		__nx_syscall_4(SYSCALL_IPC_PEEK, ret, a, b, c, d);

		return ret;
	}

	size_t poll()
	{
		size_t ret = 0;
		__nx_syscall_0(SYSCALL_IPC_POLL, ret);

		return ret;
	}

	void* install_intr_signal_handler(uint64_t sigType, signal_handler_fn_t handler)
	{
		void* ret = 0;
		__nx_syscall_2(SYSCALL_IPC_SET_SIG_HANDLER, ret, sigType, handler);

		return ret;
	}
}
}



















