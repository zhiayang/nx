// sc_process.cpp
// Copyright (c) 2021, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "loader.h"

namespace nx
{
	pid_t syscall::spawn_process_from_memory(void* buf, size_t len)
	{
		// TODO: verify this buffer!!!!!!!
		// TODO: allow a way to set the name here
		auto proc = scheduler::createProcess("anon_process", scheduler::Process::PROC_USER);

		addr_t entry = 0;
		bool success = loader::loadIndeterminateBinary(proc, buf, len, &entry);
		if(success)
		{
			scheduler::createThread(proc, (scheduler::Fn0Args_t) entry);
			return proc->processId;
		}
		else
		{
			error("sc_process", "spawn_process_from_memory failed");
			return -1;
		}
	}
}
