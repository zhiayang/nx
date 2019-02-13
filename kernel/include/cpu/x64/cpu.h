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

			void setRSP0(addr_t tssBase, uint64_t rsp0);
		}

		namespace fpu
		{
			void init();

			size_t getFPUStateSize();

			void initState(addr_t buffer);
			void save(addr_t buffer);
			void restore(addr_t buffer);

			struct sse_state
			{
				uint16_t fcw;
				uint16_t fsw;
				uint8_t  ftw;
				uint8_t  reserved;
				uint16_t fop;

				uint64_t fip;
				uint64_t fdp;

				uint32_t mxcsr;
				uint32_t mxcsrMask;

				struct {
					uint64_t low;
					uint64_t high;
				} mmxRegs[8];

				struct {
					uint64_t low;
					uint64_t high;
				} xmmRegs[16];

			} __attribute__((packed));

			struct xsave_state
			{
				sse_state fxsaveRegion;

				uint8_t reserved[96];

				// xsave header
				uint64_t xstateBV;
				uint64_t xcompBV;
				uint8_t reserved1[48];

				uint8_t extendedRegion[];

			} __attribute__((packed));
		}

		#else

		#endif
	}
}
























