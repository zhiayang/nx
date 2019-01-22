// scheduler.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace scheduler
{
	extern "C" int nx_x64_scheduler_tick()
	{
		return 0;
	}


	extern "C" void nx_x64_find_and_switch_thread(uint64_t stackPointer)
	{
	}
}
}




























