// process.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace scheduler
{
	static pid_t processIdCounter = 1;
	Process* createProcess(const nx::string& name, int flags)
	{
		auto proc = new Process();

		proc->processId = processIdCounter++;
		proc->processName = name;
		proc->flags = flags;

		// allocate a cr3 for it.
		proc->cr3 = pmm::allocate(1);

		vmm::init(proc);

		return proc;
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
}
}























