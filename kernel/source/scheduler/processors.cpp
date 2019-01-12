// processors.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace scheduler
{
	struct Processor
	{
		int id;
		int lApicId;

		bool isBootstrap;
		addr_t localApicAddr;
	};


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
		Processor p;
		p.id = id;
		p.lApicId = lApicId;
		p.localApicAddr = localApic;

		p.isBootstrap = bsp;
		processors.append(p);
	}
}
}