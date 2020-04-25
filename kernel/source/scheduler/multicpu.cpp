// multicpu.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"


namespace nx {
namespace scheduler
{
	void initCPU(CPU* cpu)
	{
		assert(cpu);
		kassert(cpu->localState == 0, "cpu %d was already initialised!", cpu->id);

		// we can only init cpus before the scheduler starts, and while we are in the kernel's address space.
		assert(getCurrentProcess() == getKernelProcess());

		// make the local state struct.
		cpu->localState = new CPULocalState();
		cpu->localState->cpu        = cpu;
		cpu->localState->id         = cpu->id;
		cpu->localState->lApicId    = cpu->lApicId;
		cpu->localState->self       = cpu->localState;

		// make a tss
		auto [ tssbase, tsssel ] = cpu::tss::createTSS();
		cpu->localState->tssSelector    = tsssel;
		cpu->localState->TSSBase        = tssbase;

		if(cpu->isBootstrap)
		{
			// note: %gs always uses the value in MSR_GS_BASE; swapgs just swaps it with the other MSR.
			// so, since we call this while in kernel-space, we write the local state to MSR_GS_BASE;
			// when we exit to userspace, we will 'swap' to the user gs.

			// write gsbase to point to the cpu local state.
			cpu::writeMSR(cpu::MSR_GS_BASE, (uint64_t) cpu->localState);
			cpu::writeMSR(cpu::MSR_KERNEL_GS_BASE, 0);

			cpu::tss::loadTSS((uint16_t) cpu->localState->tssSelector);

			// now that the TSS is loaded, we can use ISTs...
			exceptions::initWithISTs();

			// finally, setup our lazy page fault handler.
			exceptions::setupPageFaultHandler();
		}
		else
		{
			// TODO: send IPIs to wake up the APs.
		}
	}

	extern "C" uint64_t nx_x64_get_gs_base();
	CPULocalState* getCPULocalState()
	{
		return (CPULocalState*) nx_x64_get_gs_base();
	}





































	static array<CPU> processors;

	void preinitCPUs()
	{
		new (&processors) array<CPU>();
		setInitPhase(SchedulerInitPhase::ReadyToRegisterCPUs);
	}

	nx::array<CPU>& getAllCPUs()
	{
		return processors;
	}

	size_t getNumCPUs()
	{
		return processors.size();
	}

	void registerCPU(bool bsp, int id, int lApicId, addr_t localApic)
	{
		assert(getInitPhase() >= SchedulerInitPhase::ReadyToRegisterCPUs);

		// make sure the bsp is the first one on the list!!
		// the MADT tables from ACPI guarantee this.
		assert(!bsp || processors.empty());

		processors.emplace(id, lApicId, localApic, bsp);
		setInitPhase(SchedulerInitPhase::BootstrapCPURegistered);
	}

	CPU* getCurrentCPU()
	{
		if(getInitPhase() >= SchedulerInitPhase::SchedulerStarted)
			return getCPULocalState()->cpu;

		else
			return &processors[0];
	}
}
}


























