// ipc.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <syscall.h>
#include <nx/thread_local.h>

#include <stdio.h>

#include "ipc.h"

namespace nx {
namespace ipc
{
	int send(uint64_t target, const message_body_t* body)
	{
		int64_t ret = 0;
		__nx_syscall_2(SYSCALL_IPC_SEND, ret, target, body);

		return ret;
	}

	void discard()
	{
		__nx_syscall_0v(SYSCALL_IPC_DISCARD);
	}

	uint64_t peek(message_body_t* msg)
	{
		uint64_t ret = 0;
		__nx_syscall_1(SYSCALL_IPC_PEEK, ret, msg);

		return ret;
	}

	uint64_t receive(message_body_t* msg)
	{
		uint64_t ret = 0;
		__nx_syscall_1(SYSCALL_IPC_RECEIVE, ret, msg);

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


	uint64_t create_memory_ticket(size_t len, uint64_t flags)
	{
		uint64_t ret = 0;
		__nx_syscall_2(SYSCALL_MEMTICKET_CREATE, ret, len, flags);

		return ret;
	}

	mem_ticket_t collect_memory_ticket(uint64_t ticketId)
	{
		mem_ticket_t ticket { 0 };
		__nx_syscall_2v(SYSCALL_MEMTICKET_COLLECT, &ticket, ticketId);

		return ticket;
	}

	void release_memory_ticket(const mem_ticket_t& ticket)
	{
		__nx_syscall_1v(SYSCALL_MEMTICKET_RELEASE, &ticket);
	}
}
}



















