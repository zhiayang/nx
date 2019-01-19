// processors.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace scheduler
{
	static bool InitialisedAPs = false;
	static array<Processor> processors;

	void preinitProcs()
	{
		processors = array<Processor>();
	}

	size_t getNumProcessors()
	{
		return processors.size();
	}

	void registerProcessor(bool bsp, int id, int lApicId, addr_t localApic)
	{
		// make sure the bsp is the first one on the list!!
		// the MADT tables from ACPI guarantee this.
		assert(!bsp || processors.empty());

		Processor p;
		p.id = id;
		p.lApicId = lApicId;
		p.localApicAddr = localApic;

		p.isBootstrap = bsp;
		processors.append(p);
	}

	Processor* getCurrentProcessor()
	{
		if(!InitialisedAPs) return &processors[0];
		else                abort("!");
	}
}
}