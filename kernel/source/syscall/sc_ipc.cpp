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

	uint64_t syscall::sc_ipc_receive(uint64_t* a, uint64_t* b, uint64_t* c, uint64_t* d)
	{
		// note: there is no race condition here cos new messages go to the back, and discard pops from the front.
		auto ret = sc_ipc_peek(a, b, c, d);
		if(ret) sc_ipc_discard();

		return ret;
	}

	uint64_t syscall::sc_ipc_peek(uint64_t* a, uint64_t* b, uint64_t* c, uint64_t* d)
	{
		auto proc = scheduler::getCurrentProcess();
		assert(proc);

		if(proc->pendingMessages.empty())
			return 0;

		auto msg = proc->pendingMessages.front();
		if(a)   *a = msg.body.a;
		if(b)   *b = msg.body.b;
		if(c)   *c = msg.body.c;
		if(d)   *d = msg.body.d;

		return msg.senderId;
	}

	int64_t syscall::sc_ipc_send(uint64_t target, uint64_t a, uint64_t b, uint64_t c, uint64_t d)
	{
		if(target == 0) return -1;
		auto senderId = (uint64_t) scheduler::getCurrentProcess()->processId;

		// ok, add it.
		addMessage(message_t {
			.senderId   = senderId,
			.targetId   = target,
			.flags      = 0,
			.body = message_body_t {
				a, b, c, d
			}
		});

		return 0;
	}



	void syscall::sc_user_signal_leave()
	{
		// do nothing?
		auto thr = scheduler::getCurrentThread();
		assert(thr);

		log("ipc", "thread %lu finished signalled message", thr->threadId);

		thr->pendingSignalRestore = true;
		scheduler::yield();
	}
}












