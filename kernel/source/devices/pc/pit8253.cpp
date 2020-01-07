// pit8253.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "devices/pc/pit8253.h"

// #include "math.h"

namespace nx {
namespace device {
namespace pit8253
{
	// channel 1 probably doesn't exist, and channel 2 is usually wired to the speaker
	// leaving us with channel 0 to actually time stuff.
	constexpr int PIT_CHANNEL_0         = 0x40;
	constexpr int PIT_COMMAND           = 0x43;

	constexpr int BASE_FREQUENCY_HZ     = 1193182;  // 1.1931816666 MHz

	constexpr int MINIMUM_SAFE_DIVISOR  = 2400;
	constexpr int FREQUENCY_DIVISOR     = 8000;

	constexpr int TICK_FREQUENCY        = BASE_FREQUENCY_HZ / FREQUENCY_DIVISOR;
	constexpr uint64_t NS_PER_TICK      = 1'000'000'000.0 / TICK_FREQUENCY;

	static void setDivisor(uint16_t d)
	{
		port::write1b(PIT_COMMAND, 0x34);
		port::write1b(PIT_CHANNEL_0, (d >> 0 & 0xFE));  // use even divisors
		port::write1b(PIT_CHANNEL_0, (d >> 8 & 0xFF));
	}

	void enable()
	{
		setDivisor((uint16_t) FREQUENCY_DIVISOR);
	}

	void setPeriod(uint64_t ns)
	{
		auto freq = time::seconds(1).ns() / ns;
		auto divisor = (uint64_t) BASE_FREQUENCY_HZ / freq;

		if(divisor < MINIMUM_SAFE_DIVISOR)
		{
			divisor = MINIMUM_SAFE_DIVISOR;
			warn("pit8253", "clamping divisor to %u (%.2f hz)", (double) BASE_FREQUENCY_HZ / (double) divisor);
		}
		else if(divisor > 65535)
		{
			divisor = 65535;
			warn("pit8253", "clamping divisor to %u (%.2f hz)", (double) BASE_FREQUENCY_HZ / (double) divisor);
		}

		setDivisor((uint16_t) divisor);
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






















