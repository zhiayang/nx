// process.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace scheduler
{
	static pid_t processIdCounter = 0;
	static nx::mutex processListLock;
	static nx::list<Process*> allProcesses;

	Process* createProcess(const nx::string& name, int flags)
	{
		if(processIdCounter == 0)
			allProcesses = nx::list<Process*>();

		auto proc = new Process();
		proc->addrspace.lock()->init();

		proc->processId = ++processIdCounter;
		proc->processName = name;
		proc->flags = flags;

		// allocate an ioctx
		proc->ioctx = new vfs::IOCtx();

		vmm::init(proc);

		LockedSection(&processListLock, [&proc]() {
			allProcesses.append(proc);
		});

		log("sched", "created process '%s' (pid: %lu, cr3: %p)", proc->processName.cstr(), proc->processId, proc->addrspace.cr3());
		return proc;
	}

	void destroyProcess(Process* proc)
	{
		assert(proc);

		assert(proc->ioctx);
		delete proc->ioctx;

		vmm::destroy(proc);
		proc->addrspace.lock()->destroy();

		// TODO: remove it from its cpu list as well
		LockedSection(&processListLock, [&proc]() {
			allProcesses.remove_all(proc);
		});

		getSchedState()->ProcessList.remove_all(proc);

		log("sched", "destroyed process '%s' (pid: %lu)", proc->processName.cstr(), proc->processId);
		delete proc;
	}









	static Process KernelProcess;
	static CPULocalState bootstrapLocalState;

	void setupKernelProcess(PhysAddr cr3)
	{
		// setup the bootstrap local state, so we can dereference %gs
		new (&bootstrapLocalState) CPULocalState();

		// the rest can be null.
		bootstrapLocalState.self = &bootstrapLocalState;

		cpu::writeMSR(cpu::MSR_GS_BASE, (uint64_t) &bootstrapLocalState);
		cpu::writeMSR(cpu::MSR_KERNEL_GS_BASE, 0);


		// now we can do stuff.

		new (&KernelProcess) Process();
		KernelProcess.processId = 0;
		KernelProcess.addrspace.lock()->init(cr3);

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
		for(auto p : allProcesses)
			if(p->processId == id)
				return p;

		return nullptr;
	}


	Thread* getThreadWithId(pid_t id)
	{
		// oh no x2
		for(auto p : allProcesses)
			for(auto& t : p->threads)
				if(t.threadId == id)
					return &t;

		return nullptr;
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























