// ticker.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "misc/timekeeping.h"

namespace nx {
namespace scheduler
{
	static int TickIRQ = 0;
	void setTickIRQ(int irq)
	{
		TickIRQ = irq;
	}


	static uint64_t ElapsedNanoseconds = 0;
	uint64_t getElapsedNanoseconds()
	{
		return ElapsedNanoseconds;
	}


	static constexpr uint64_t NS_PER_TICK           = time::milliseconds(500).ns();
	static constexpr uint64_t TIMESLICE_DURATION_NS = time::milliseconds(1000).ns();

	static_assert(NS_PER_TICK <= TIMESLICE_DURATION_NS);

	uint64_t getNanosecondsPerTick()
	{
		return NS_PER_TICK;
	}


	// returns true if it's time to preempt somebody
	static uint64_t TickCounter = 0;
	extern "C" int nx_x64_scheduler_tick()
	{
		interrupts::sendEOI(TickIRQ);

		TickCounter += 1;
		ElapsedNanoseconds += NS_PER_TICK;

		bool ret = (TickCounter * NS_PER_TICK) >= TIMESLICE_DURATION_NS;
		if(ret) TickCounter = 0;

		return ret;
	}
}
}

























