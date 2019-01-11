// interrupts.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

namespace nx
{
	namespace interrupts
	{
		// everybody needs an init
		void init();

		#ifdef __ARCH_x64__

		void setInterruptGate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags);

		#else

		#endif
	}
}