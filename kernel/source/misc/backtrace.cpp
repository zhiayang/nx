// backtrace.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "elf.h"

#define NX_BOOTINFO_VERSION NX_MAX_BOOTINFO_VERSION
#include "bootinfo.h"

namespace nx {
namespace util
{
	nx::array<addr_t> getStackFrames(uint64_t _rbp)
	{
		array<addr_t> addrs;

		// fetch rbp
		addr_t rbp = (_rbp ? _rbp : (addr_t) __builtin_frame_address(0));

		do {
			auto rip = *((addr_t*) (rbp + sizeof(uint64_t)));
			if(!IS_CANONICAL(rip)) break;

			if(rip) addrs.append(rip);

			rbp = *((addr_t*) rbp);

		} while(rbp);

		return addrs;
	}

	static bool nested = false;
	void printStackTrace(uint64_t _rbp, scheduler::Process* proc)
	{
		serial::debugprintf("backtrace:\n");
		if(nested)
		{
			serial::debugprintf("aborting backtrace!\n");
			return;
		}

		if(!proc)
			proc = scheduler::getCurrentProcess();

		nested = true;

		addr_t rbp = (_rbp ? _rbp : (addr_t) __builtin_frame_address(0));

		while(rbp)
		{
			auto rip = *((addr_t*) (rbp + sizeof(uint64_t)));
			if(!IS_CANONICAL(rip)) break;

			if(rip)
			{
				if(heap::initialised())
				{
					const auto& s = syms::symbolicate(rip, proc);
					serial::debugprintf("  %8lx  |  %s\n", rip, s.cstr());
				}
				else
				{
					serial::debugprintf("  %8lx\n", rip);
				}
			}

			rbp = *((addr_t*) rbp);
		}

		nested = false;
		serial::debugprintf("\n");
	}




	void initKernelSymbols(BootInfo* bi)
	{
		if(bi->version < 2)
		{
			warn("backtrace", "bootinfo version '%d' does not support symbol loading", bi->version);
			return;
		}

		// ok, do something.
		assert(bi->kernelElf);
		assert(bi->kernelElfSize);

		// make sure we can demangle shit.
		initDemangler();

		syms::readSymbolsFromELF(bi->kernelElf, bi->kernelElfSize, scheduler::getKernelProcess());

		log("backtrace", "found %s", util::plural("symbol", scheduler::getKernelProcess()->symbols.size()).cstr());
		pmm::freeEarlyMemory(bi, MemoryType::KernelElf);
	}
}
}





























