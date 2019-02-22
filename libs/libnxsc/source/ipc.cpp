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
	int send(uint64_t target, void* buf, size_t len)
	{
		int64_t ret = 0;
		__nx_syscall_3(SYSCALL_IPC_SEND, ret, target, buf, len);

		return ret;
	}

	void discard()
	{
		__nx_syscall_0v(SYSCALL_IPC_DISCARD);
	}

	size_t peek(void* buf, size_t len)
	{
		size_t ret = 0;
		__nx_syscall_2(SYSCALL_IPC_PEEK, ret, buf, len);

		return ret;
	}

	size_t receive(void* buf, size_t len)
	{
		size_t ret = 0;
		__nx_syscall_2(SYSCALL_IPC_RECEIVE, ret, buf, len);

		return ret;
	}

	size_t poll()
	{
		size_t ret = 0;
		__nx_syscall_0(SYSCALL_IPC_POLL, ret);

		return ret;
	}
}
}



















