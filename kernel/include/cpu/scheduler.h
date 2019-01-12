// scheduler.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

#include "defs.h"

namespace nx
{
	namespace scheduler
	{
		void preinitProcs();
		void registerProcessor(bool bsp, int id, int lApicId, addr_t localApic);

		size_t getNumProcessors();
	};
}