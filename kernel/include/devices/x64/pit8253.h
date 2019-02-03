// pit8253.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "defs.h"

namespace nx
{
	namespace device
	{
		namespace pit8253
		{
			void enable();
			void disable();

			void tick();
			uint64_t getTicks();

			uint64_t getNanosecondsPerTick();
		}
	}
}