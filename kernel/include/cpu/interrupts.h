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
		void init();
		void enable();
		void disable();

		void maskIRQ(int num);
		void unmaskIRQ(int num);

		// note: 'num' is in IRQ space! (0 => IRQ_BASE_VECTOR)
		void sendEOI(int num);

		bool hasIOAPIC();
		bool hasLocalAPIC();

		// note: 'vector' is in IRQ space! (0 => IRQ_BASE_VECTOR)
		void mapIRQVector(int irq, int vector, int apicId);
	}
}