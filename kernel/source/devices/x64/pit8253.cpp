// pit8253.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "devices/x64/pit8253.h"

#include "math.h"

namespace nx {
namespace device {
namespace pit8253
{
	// channel 1 probably doesn't exist, and channel 2 is usually wired to the speaker
	// leaving us with channel 0 to actually time stuff.
	static constexpr int PIT_CHANNEL_0          = 0x40;
	static constexpr int PIT_COMMAND            = 0x43;

	// 1.1931816666 MHz
	static constexpr int BASE_FREQUENCY_HZ      = 1193182;
	static constexpr int FREQUENCY_DIVISOR      = 82;       // 1193182 / 82 = 14551

	static constexpr int TICK_FREQUENCY         = BASE_FREQUENCY_HZ / FREQUENCY_DIVISOR;
	static constexpr double SECONDS_PER_TICK    = 1.0 / (double) TICK_FREQUENCY;

	static constexpr double NS_PER_TICK         = SECONDS_PER_TICK * 1.0e9;

	void enable()
	{
		// we just let this sit at a fixed (high-ish) frequency, then use that to keep time accurate to the ms.
		auto d = (uint16_t) FREQUENCY_DIVISOR;

		port::write1b(PIT_COMMAND, 0x36);
		port::write1b(PIT_CHANNEL_0, d & 0xFF);
		port::write1b(PIT_CHANNEL_0, (d & 0xFF00) >> 8);
	}

	void disable()
	{
		// set it to one-shot mode
		port::write1b(PIT_COMMAND, 0x30);
		port::write1b(PIT_CHANNEL_0, 0xFF);
		port::write1b(PIT_CHANNEL_0, 0xFF);
	}

	static uint64_t TickCounter = 0;
	void tick()                         { TickCounter += 1; }
	uint64_t getTicks()                 { return TickCounter; }
	uint64_t getNanosecondsPerTick()    { return (uint64_t) round(NS_PER_TICK); }
}
}
}






















