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

		void sendEOI(int num);

		bool hasIOAPIC();

		// note: 'vector' is in IDT space! (please use vectors >= 32)
		void mapIRQVector(int irq, int vector, int apicId);
	}

	namespace cpu
	{
		#ifdef __ARCH_x64__
		namespace idt
		{
			void init();
			void setEntry(uint8_t intr, addr_t fn, uint16_t codeSegment, uint8_t flags);
			void clearEntry(uint8_t intr);

			void enableGate(uint8_t intr);
			void disableGate(uint8_t intr);
		}
		#else
		#endif
	}
}