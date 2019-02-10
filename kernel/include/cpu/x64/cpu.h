// x64/cpu.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include "defs.h"

namespace nx
{
	namespace cpu
	{
		#ifdef __ARCH_x64__
		namespace idt
		{
			void init();
			void setEntry(uint8_t intr, addr_t fn, uint16_t codeSegment, bool ring3Interrupt, bool nestedInterrupts);
			void clearEntry(uint8_t intr);

			void enableGate(uint8_t intr);
			void disableGate(uint8_t intr);
		}

		namespace gdt
		{
			void init();

			// returns (virt, phys)
			krt::pair<addr_t, addr_t> getGDTAddress();

			size_t getNextSelectorIndex();
			void incrementSelectorIndex(size_t ofs);
		}

		namespace tss
		{
			// returns (base, selector) of the created tss
			krt::pair<addr_t, uint16_t> createTSS();

			void loadTSS(uint16_t selector);
		}

		#else

		#endif
	}
}









