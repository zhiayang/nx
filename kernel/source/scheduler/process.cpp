// process.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace scheduler
{
	static pid_t processIdCounter = 1;
	static nx::list<Process*> AllProcesses;

	Process* createProcess(const nx::string& name, int flags)
	{
		if(processIdCounter == 1)
			AllProcesses = nx::list<Process*>();


		auto proc = new Process();

		proc->processId = processIdCounter++;
		proc->processName = name;
		proc->flags = flags;

		// allocate a cr3 for it.
		proc->cr3 = pmm::allocate(1);

		vmm::init(proc);

		AllProcesses.append(proc);

		return proc;
	}

	void destroyProcess(Process* proc)
	{
		assert(proc);

		vmm::destroy(proc);
		pmm::deallocate(proc->cr3, 1);

		// TODO: remove it from its cpu list as well

		for(auto it = AllProcesses.begin(); it != AllProcesses.end(); ++it)
		{
			if(*it == proc)
			{
				AllProcesses.erase(it);
				break;
			}
		}

		delete proc;
	}










	static Process KernelProcess;
	void setupKernelProcess(addr_t cr3)
	{
		KernelProcess = Process();
		KernelProcess.cr3 = cr3;
		KernelProcess.processId = 0;

		setInitPhase(SchedulerInitPhase::KernelProcessInit);
	}

	Process* getKernelProcess()
	{
		return &KernelProcess;
	}

	Process* getCurrentProcess()
	{
		assert(getInitPhase() > SchedulerInitPhase::Uninitialised);

		if(__unlikely(getInitPhase() < SchedulerInitPhase::BootstrapCPURegistered))
			return &KernelProcess;

		auto cpu = getCurrentCPU();
		if(__unlikely(!cpu->currentProcess))
			return &KernelProcess;

		return cpu->currentProcess;
	}

	Process* getProcessWithId(pid_t id)
	{
		// oh no
		for(auto p : AllProcesses)
			if(p->processId == id)
				return p;

		return 0;
	}
}
}























