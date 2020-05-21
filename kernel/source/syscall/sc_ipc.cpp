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

	// TODO: since the selector contains a pointer to a userspace string, we most
	// likely need to sanitise this as well. sad reacts.

	void syscall::ipc_signal(ipc::selector_t sel, uint64_t a, uint64_t b, uint64_t c)
	{
		auto smb = signal_message_body_t(a, b, c);
		ipc::signal(sel, ipc::SIGNAL_NORMAL, smb);
	}

	void syscall::ipc_signal_block(ipc::selector_t sel, uint64_t a, uint64_t b, uint64_t c)
	{
		auto smb = signal_message_body_t(a, b, c);

		// this condvar needs to live on the kernel heap, because any process should be able
		// to set/reset it. we can just free it later, no big deal.
		auto cv = new condvar<bool>();
		if(!ipc::signalBlocking(sel, ipc::SIGNAL_NORMAL, smb, cv))
			return;

		cv->wait(false);
		delete cv;
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












