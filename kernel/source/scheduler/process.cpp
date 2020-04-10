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
		proc->addrspace.init();

		proc->processId = processIdCounter++;
		proc->processName = name;
		proc->flags = flags;


		// allocate an ioctx
		proc->ioctx = new vfs::IOCtx();

		vmm::init(proc);

		AllProcesses.append(proc);

		log("sched", "created process '%s' (pid: %lu, cr3: %p)", proc->processName.cstr(), proc->processId, proc->addrspace.cr3);
		return proc;
	}

	void destroyProcess(Process* proc)
	{
		assert(proc);

		assert(proc->ioctx);
		delete proc->ioctx;

		vmm::destroy(proc);
		proc->addrspace.destroy();

		// TODO: remove it from its cpu list as well
		AllProcesses.remove_all(proc);
		getSchedState()->ProcessList.remove_all(proc);

		log("sched", "destroyed process '%s' (pid: %lu)", proc->processName.cstr(), proc->processId);
		delete proc;
	}










	static Process KernelProcess;
	void setupKernelProcess(PhysAddr cr3)
	{
		KernelProcess = Process();
		KernelProcess.processId = 0;
		KernelProcess.addrspace.init(cr3);

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
		assert(cpu);

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



	void allowProcessIOPorts(Process* p, const nx::array<krt::pair<uint16_t, size_t>>& allowedPorts)
	{
		for(auto [ port, num ] : allowedPorts)
		{
			for(size_t i = 0; i < num; i++)
				cpu::tss::setIOPortPerms(&p->ioPorts, port + i, true);
		}
	}

	void allowProcessIOPort(Process* p, uint16_t port)
	{
		cpu::tss::setIOPortPerms(&p->ioPorts, port, true);
	}
}
}























