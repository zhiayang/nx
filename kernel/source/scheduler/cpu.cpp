// processors.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace scheduler
{
	static bool InitialisedAPs = false;
	static array<CPU> processors;

	void preinitCPUs()
	{
		processors = array<CPU>();

		initialisePhase(2);
	}

	size_t getNumCPUs()
	{
		return processors.size();
	}

	void registerCPU(bool bsp, int id, int lApicId, addr_t localApic)
	{
		assert(getCurrentInitialisationPhase() >= 2);

		// make sure the bsp is the first one on the list!!
		// the MADT tables from ACPI guarantee this.
		assert(!bsp || processors.empty());

		CPU p;
		p.id = id;
		p.lApicId = lApicId;
		p.localApicAddr = localApic;

		p.isBootstrap = bsp;
		processors.append(p);

		initialisePhase(3);
	}

	CPU* getCurrentCPU()
	{
		if(!InitialisedAPs) return &processors[0];
		else                abort("!");
	}
}
}