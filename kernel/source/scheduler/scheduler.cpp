// scheduler.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace scheduler
{
	static uint64_t ticks = 0;
	extern "C" int nx_x64_scheduler_tick()
	{
		ticks += 1;
		return 0;
	}

	extern "C" void nx_x64_find_and_switch_thread(uint64_t stackPointer)
	{
	}



























	static int initPhase = 0;
	int getCurrentInitialisationPhase()
	{
		return initPhase;
	}

	void initialisePhase(int p)
	{
		assert(p >= initPhase);
		initPhase = p;
	}
}
}




























