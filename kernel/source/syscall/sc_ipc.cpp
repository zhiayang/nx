// sc_handlers.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx
{
	using namespace nx::ipc;


	void syscall::sc_ipc_discard()
	{
		auto proc = scheduler::getCurrentProcess();
		assert(proc);

		if(proc->pendingMessages.size() > 0)
		{
			autolock lk(&proc->msgQueueLock);
			auto msg = proc->pendingMessages.popFront();

			disposeMessage(msg);
		}
	}

	size_t syscall::sc_ipc_poll()
	{
		auto proc = scheduler::getCurrentProcess();
		assert(proc);

		return proc->pendingMessages.size();
	}

	size_t syscall::sc_ipc_receive(void* msg, size_t len)
	{
		// note: there is no race condition here cos new messages go to the back, and discard pops from the front.
		auto ret = sc_ipc_peek(msg, len);

		// note: we only discard if we received a proper buffer.
		if(msg && ret) sc_ipc_discard();

		return ret;
	}

	size_t syscall::sc_ipc_peek(void* buf, size_t len)
	{
		auto proc = scheduler::getCurrentProcess();
		assert(proc);

		if(proc->pendingMessages.empty())
			return 0;

		if(!buf)
			return proc->pendingMessages.front().payloadSize;

		auto msg = proc->pendingMessages.front();

		// buffer too small u dummy
		if(msg.payloadSize > len)
			return 0;

		memcpy(buf, msg.payload, msg.payloadSize);
		return msg.payloadSize;
	}

	int64_t syscall::sc_ipc_send(uint64_t target, void* buf, size_t len)
	{
		if(!buf || len == 0) return -1;

		if(len > MAX_MESSAGE_SIZE)
		{
			error("ipc", "message size of %zu exceeds max of %zu; discarding", len, MAX_MESSAGE_SIZE);
			return -1;
		}

		auto senderId = scheduler::getCurrentProcess()->processId;

		// ok, add it
		addMessage(senderId, target, buf, len);

		return 0;
	}
}












