// sc_ipc.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "cpu/interrupts.h"

namespace nx
{
	using namespace nx::ipc;


	void syscall::ipc_discard()
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

	size_t syscall::ipc_poll()
	{
		auto proc = scheduler::getCurrentProcess();
		assert(proc);

		return proc->pendingMessages.size();
	}

	uint64_t syscall::ipc_receive(message_body_t* msg)
	{
		// note: there is no race condition here cos new messages go to the back, and discard pops from the front.
		auto ret = ipc_peek(msg);
		if(ret) ipc_discard();

		return ret;
	}

	uint64_t syscall::ipc_peek(message_body_t* output)
	{
		auto proc = scheduler::getCurrentProcess();
		assert(proc);

		if(proc->pendingMessages.empty())
			return 0;

		auto msg = proc->pendingMessages.front();
		if(output) *output = msg.body;

		return msg.senderId;
	}

	int64_t syscall::ipc_send(uint64_t target, message_body_t* body)
	{
		if(target == 0) return -1;
		auto senderId = (uint64_t) scheduler::getCurrentProcess()->processId;

		// ok, add it.
		addMessage(message_t {
			.senderId   = senderId,
			.targetId   = target,
			.flags      = 0,
			.body       = *body
		});

		return 0;
	}



	// returns the old handler.
	void* syscall::ipc_set_signal_handler(uint64_t sigType, void* new_handler)
	{
		if(sigType >= MAX_SIGNAL_TYPES)
			return (void*) -1;

		auto thr = scheduler::getCurrentThread();
		auto ret = (void*) thr->signalHandlers[sigType];

		thr->signalHandlers[sigType] = (signal_handler_fn_t) new_handler;

		log("ipc", "thr %lu installed handler for sigType %lu", thr->threadId, sigType);
		return ret;
	}

	void syscall::user_signal_leave(uint64_t returnCode)
	{
		auto thr = scheduler::getCurrentThread();
		assert(thr);

		// handled:
		if(returnCode & SIGNAL_IRQ_HANDLED_FLAG)
			interrupts::signalIRQHandled((uint32_t) returnCode);

		// ignored:
		if(returnCode & SIGNAL_IRQ_IGNORED_FLAG)
			interrupts::signalIRQIgnored((uint32_t) returnCode);


		thr->pendingSignalRestore = true;
		scheduler::yield();
	}
}












