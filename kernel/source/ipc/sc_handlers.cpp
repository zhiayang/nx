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
			proc->pendingMessages.popFront();
		}
	}

	size_t syscall::sc_ipc_poll()
	{
		auto proc = scheduler::getCurrentProcess();
		assert(proc);

		return (int64_t) proc->pendingMessages.size();
	}

	size_t syscall::sc_ipc_receive(void* msg, size_t len)
	{
		// note: there is no race condition here cos new messages go to the back, and discard pops from the front.
		auto ret = sc_ipc_peek(msg, len);

		// note: we only discard if we received a proper buffer.
		if(msg && ret) sc_ipc_discard();

		return ret;
	}

	size_t syscall::sc_ipc_peek(void* _buf, size_t len)
	{
		auto proc = scheduler::getCurrentProcess();
		assert(proc);

		if(proc->pendingMessages.empty())
			return 0;

		if(!_buf)
			return proc->pendingMessages.front()->payloadSize + sizeof(message_t);


		auto buf = (message_t*) _buf;
		assert(buf);

		auto msg = proc->pendingMessages.front();
		auto totalLen = sizeof(message_t) + msg->payloadSize;
		if(totalLen > len)
			return 0;

		memcpy(buf, msg, totalLen);
		return totalLen;
	}

	int64_t syscall::sc_ipc_send(void* _msg, size_t len)
	{
		if(!_msg || len == 0) return -1;

		auto msg = (message_t*) _msg;
		assert(msg);

		if(msg->magic != MAGIC_LE)
		{
			error("ipc", "invalid magic %x in message; discarding", msg->magic);
			return -1;
		}
		if((msg->version & 0xFF) != CUR_VERSION)
		{
			error("ipc", "invalid version %d in message; discarding", msg->version & 0xFF);
			return -1;
		}
		if((msg->version & ~0xFF) != 0)
		{
			error("ipc", "invalid feature flags %x in message; discarding", msg->version & ~0xFF);
			return -1;
		}
		if(len > MAX_MESSAGE_SIZE)
		{
			error("ipc", "message size of %zu exceeds max of %zu; discarding", msg->payloadSize, MAX_MESSAGE_SIZE);
			return -1;
		}

		msg->senderId = scheduler::getCurrentProcess()->processId;

		// ok, add it
		addMessage(msg);

		return 0;
	}
}












