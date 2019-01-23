// scheduler.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

#include "defs.h"

namespace nx
{
	namespace scheduler
	{
		struct Processor
		{
			int id;
			int lApicId;

			bool isBootstrap;
			addr_t localApicAddr;
		};

		struct Thread;
		struct Process
		{
			addr_t cr3;
			nx::array<Thread*> threads;
		};

		struct Thread
		{
			addr_t userStack;
			addr_t kernelStack;

			Process* parent = 0;
		};




		void preinitProcs();
		void registerProcessor(bool bsp, int id, int lApicId, addr_t localApic);

		size_t getNumProcessors();

		Processor* getCurrentProcessor();
	};
}