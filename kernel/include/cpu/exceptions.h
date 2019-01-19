// exceptions.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

namespace nx
{
	struct BootInfo;

	namespace exceptions
	{
		void init();
		void setInterruptGate(uint8_t num, addr_t base, uint16_t sel, uint8_t flags);

		void enableGate(uint8_t intr);
		void disableGate(uint8_t intr);
	}
}