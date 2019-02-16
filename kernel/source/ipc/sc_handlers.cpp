// sc_handlers.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx
{
	using namespace nx::ipc;


	int64_t syscall::sc_ipc_discard()
	{
		return 0;
	}

	int64_t syscall::sc_ipc_poll()
	{
		auto proc = scheduler::getCurrentProcess();
		assert(proc);

		return proc->pendingMessages.size() > 0;
	}

	int64_t syscall::sc_ipc_receive(void* _msg, size_t len)
	{
		// unimplemented!
		return -1;
	}

	int64_t syscall::sc_ipc_peek(void* _msg, size_t len)
	{
		if(!_msg) return -1;

		auto msg = (message_t*) _msg;
		assert(msg);

		// unimplemented!
		return -1;
	}

	int64_t syscall::sc_ipc_send(void* _msg, size_t len)
	{
		if(!_msg) return -1;

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
		if(msg->payloadSize > MAX_PAYLOAD_SIZE)
		{
			error("ipc", "payload size of %zu exceeds max of %zu; discarding", msg->payloadSize, MAX_PAYLOAD_SIZE);
			return -1;
		}

		msg->senderId = scheduler::getCurrentProcess()->processId;

		// ok, add it
		addMessage(msg);

		return 0;
	}
}












