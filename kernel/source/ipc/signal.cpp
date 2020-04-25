// signal.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx
{
	using Thread = scheduler::Thread;
	using Process = scheduler::Process;

	namespace ipc
	{
		/*
			here's a high-level description of the mechanism for interrupted signalling:

			1. some process calls a syscall which forwards to signalProcess.
			2. the target process (possibly the same process) is checked for signal handlers.
				a. if there's no handler, and it's a critical signal, then terminate the process.
				b. if there's no handler, but it's a normal signal, then just discard it.

			3. the signal is added to a list of pending signals so that it can be handled conveniently
				during the next context switch into the function.

			4. we save the a copy of the thread's registers to a small region in kernel space.
				b. the save state should itself be stored on a (very small) stack, to enable
					nested signals.

			5. signalProcess tells the thread manager to modify the user stack. we set the following:
				1) change the return %rip to point to a stub function in libnxsc (eg. signal_user_enter)
				2) fix %rdi, %rsi, and %rdx to be the 3 arguments for the signal handler.
				3) %rcx to point to the actual signal handler.
				4) if requested, we can set %rsp to point to a user-specified stack. (sigaltstack)

			6. the process is context-switched into. iret puts us at signal_user_enter.
				a. we call the signal handler with its arguments.
				b. after the signal handler returns, we do a syscall that is basically sigreturn.
				c. at this point, whatever is on the stack is basically useless and needs to be trashed.

			7. at the sigreturn syscall, we set a flag that we are pending a signal restore, and suspend
				the thread.

			8. on the next context switch into the thread, we check the pending_restore flag, and if true,
				restore the saved state from (4). since we stored the whole thing, there's no need to do
				any patching -- just replace the existing stuff in the kernel stack with the saved info.

			9. we context switch back into the thread, and nobody is any the wiser.



			note: the reason why all of the stack-based operations are "deferred" to the next schedule is
			so that we don't have to mess around with having the syscall entry point push/pop all registers.
			if we just wait till the next schedule event, we know for sure that we have the complete state
			of the thread to save/restore.
		*/

		static void enqueue_signal(Thread* thr, uint64_t sigType, const signal_message_body_t& umsg)
		{
			// first, construct the final message to send:
			auto msg = signal_message_t();

			msg.targetId        = thr->threadId;
			msg.flags           = MSG_FLAG_SIGNAL;
			msg.senderId        = scheduler::getCurrentThread()->threadId;
			msg.body.sigType    = sigType;

			memcpy(msg.body.bytes, umsg.bytes, sizeof(umsg.bytes));

			// ok, we have the message.
			// TODO: needs locking!!
			thr->pendingSignalQueue.append(msg);

			// boost it a bit.
			thr->priority.boost();

			// that's it for this function.
		}

		// the default action here if there is no handler installed is to discard the message.
		bool signalThread(Thread* thr, uint64_t sigType, const signal_message_body_t& msg)
		{
			if(sigType >= MAX_SIGNAL_TYPES)
				return false;

			// check if there's a handler for it:
			auto handler = thr->signalHandlers[sigType];
			if(!handler)
			{
				log("ipc", "thread %lu has no handle for signal %lu", thr->threadId, sigType);
				return false;
			}

			enqueue_signal(thr, sigType, msg);
			return true;
		}

		// the default action here is to terminate the thread if there's no handler installed.
		bool signalThreadCritical(Thread* thr, uint64_t sigType, const signal_message_body_t& msg)
		{
			assert(thr);

			// yo wtf
			if(sigType >= MAX_SIGNAL_TYPES)
				return false;

			// check if there's a handler for it:
			auto handler = thr->signalHandlers[sigType];
			if(!handler)
			{
				// terminate that mofo.
				log("ipc", "thread %lu did not handle critical signal %lu -- terminating",
					thr->threadId, sigType);

				// the scheduler will handle the case where we end up needing to terminate ourselves.
				scheduler::terminate(thr);
				return false;
			}

			// ok, time to call the handler i guess?
			enqueue_signal(thr, sigType, msg);
			return true;
		}


		// TODO:
		// we don't support signalling processes "directly", for now. a stopgap measure is that
		// we signal the first thread in the process, always.
		void signalProcess(Process* proc, uint64_t sigType, const signal_message_body_t& msg)
		{
			signalThread(&proc->threads[0], sigType, msg);
		}

		void signalProcessCritical(Process* proc, uint64_t sigType, const signal_message_body_t& msg)
		{
			signalThreadCritical(&proc->threads[0], sigType, msg);
		}
	}
}















