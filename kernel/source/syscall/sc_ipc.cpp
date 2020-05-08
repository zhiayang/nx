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

		proc->pendingMessages.perform([&proc](auto& queue) {
			if(!queue.empty())
			{
				auto msg = queue.popFront();
				disposeMessage(msg);
			}
		});
	}

	size_t syscall::ipc_poll()
	{
		auto proc = scheduler::getCurrentProcess();
		assert(proc);

		return proc->pendingMessages.unsafeGet()->size();
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

		return proc->pendingMessages.map([&proc, &output](auto& queue) -> uint64_t {
			if(queue.empty())
				return 0;

			auto msg = queue.front();
			if(!copy_to_user(output, &msg.body, sizeof(message_body_t)))
				return 0;

			return msg.senderId;
		});
	}

	int64_t syscall::ipc_send(uint64_t target, message_body_t* body)
	{
		if(target == 0) return -1;
		auto senderId = (uint64_t) scheduler::getCurrentProcess()->processId;

		// ok, add it.
		auto msg = message_t {
			.senderId   = senderId,
			.targetId   = target,
			.flags      = 0,
			.body       = { }
		};

		if(!copy_to_kernel(&msg.body, body, sizeof(ipc::message_body_t)))
			return 0;

		addMessage(msg);
		return 0;
	}


	void syscall::ipc_signal(uint64_t target, uint64_t a, uint64_t b, uint64_t c)
	{
		// auto thr = scheduler::get
		// /process/name/kekw
		// /process/id/123
		// /thread/id/123

		auto smb = signal_message_body_t(a, b, c);
		// ipc::signalThread();
	}

	void syscall::ipc_signal_block(uint64_t target, uint64_t a, uint64_t b, uint64_t c)
	{

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












