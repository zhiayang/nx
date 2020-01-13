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

			void updateIOPB(addr_t tssBase, const nx::treemap<uint16_t, uint8_t>& ports);
			void setIOPortPerms(nx::treemap<uint16_t, uint8_t>* iopb, uint16_t port, bool allowed);
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

		// keep this in sync with the push order in context switching!!
		struct InterruptedState
		{
			// in reverse order:
			// pushed by us:  rdi, rsi, rbp, rax, rbx, rcx, rdx, r8, r9, r10, r11, r12, r13, r14, r15
			// pushed by cpu: return_addr, code_seg, rflags, rsp, stack_seg

			uint64_t rdi;
			uint64_t rsi;
			uint64_t rbp;
			uint64_t rax;
			uint64_t rbx;
			uint64_t rcx;
			uint64_t rdx;
			uint64_t r8;
			uint64_t r9;
			uint64_t r10;
			uint64_t r11;
			uint64_t r12;
			uint64_t r13;
			uint64_t r14;
			uint64_t r15;

			uint64_t rip;
			uint64_t cs;
			uint64_t rflags;
			uint64_t rsp;
			uint64_t ss;

			void dump() const;
		};

		// this is the same as the interrupted state, but includes cr2, the interrupt id,
		// and the error code.
		struct ExceptionState
		{
			uint64_t cr2;
			uint64_t rsp;
			uint64_t rdi, rsi, rbp;
			uint64_t rax, rbx, rcx, rdx, r8, r9, r10, r11, r12, r13, r14, r15;

			uint64_t InterruptID, ErrorCode;
			uint64_t rip, cs, rflags, useresp, ss;

			void dump() const;
		};

		#else

		#endif
	}
}
























