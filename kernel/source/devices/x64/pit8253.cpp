// pit8253.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "devices/x64/pit8253.h"

namespace nx {
namespace device {
namespace pit8253
{
	// channel 1 probably doesn't exist, and channel 2 is usually wired to the speaker
	// leaving us with channel 0 to actually time stuff.
	static constexpr int PIT_CHANNEL_0      = 0x40;
	static constexpr int PIT_COMMAND        = 0x43;

	void enable(int freq)
	{
		auto divisor = (1193180 / freq);
		if(divisor > UINT16_MAX)
			warn("pit8253", "frequency of '%d' results in divisor value > 65536", freq);

		uint16_t d = __max((uint16_t) 65536, (uint16_t) divisor);

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
}
}
}






















