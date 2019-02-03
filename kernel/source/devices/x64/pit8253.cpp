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

	static constexpr int BASE_FREQUENCY_HZ      = 1193182;  // 1.1931816666 MHz
	static constexpr int FREQUENCY_DIVISOR      = 8000;

	static constexpr int TICK_FREQUENCY         = BASE_FREQUENCY_HZ / FREQUENCY_DIVISOR;
	static constexpr uint64_t NS_PER_TICK       = 1'000'000'000.0 / TICK_FREQUENCY;

	void enable()
	{
		// we just let this sit at a fixed (high-ish) frequency, then use that to keep time accurate to the ms.
		auto d = (uint16_t) FREQUENCY_DIVISOR;

		port::write1b(PIT_COMMAND, 0x34);
		port::write1b(PIT_CHANNEL_0, (d >> 0 & 0xFE));  // use even divisors
		port::write1b(PIT_CHANNEL_0, (d >> 8 & 0xFF));
	}

	void disable()
	{
		// set it to one-shot mode
		port::write1b(PIT_COMMAND, 0x28);
		port::write1b(PIT_CHANNEL_0, 0);
	}

	static uint64_t TickCounter = 0;
	void tick()                         { TickCounter++; }
	uint64_t getTicks()                 { return TickCounter; }
	uint64_t getNanosecondsPerTick()    { return NS_PER_TICK; }
}
}
}






















