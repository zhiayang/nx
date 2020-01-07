// signal.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx
{
	using Process = scheduler::Process;

	namespace ipc
	{
		// the default action here if there is no handler installed is to discard the message.
		void signalProcess(Process* proc, uint64_t sigType, const message_t& msg)
		{

		}

		// the default action here is to terminate the process if there's no handler installed.
		void signalProcessCritical(Process* proc, uint64_t sigType, const message_t& msg)
		{
			assert(proc);

			// yo wtf
			if(sigType >= MAX_SIGNAL_TYPES)
				return;

			// check if there's a handler for it:
			auto handler = proc->signalHandlers[sigType];
			if(!handler)
			{
				// terminate that mofo.
				log("ipc", "process %lu did not handle critical signal %lu -- terminating",
					proc->processId, sigType);

				// the scheduler will handle the case where we end up needing to terminate ourselves.
				scheduler::terminate(proc);
			}

			// ok, time to call the handler i guess?
		}
	}
}















