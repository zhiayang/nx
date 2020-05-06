// util.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "defs.h"

namespace nx
{
	struct BootInfo;

	namespace syms
	{
		struct Symbol
		{
			addr_t addr;
			size_t size;
			nx::string name;
		};

		void readSymbolsFromELF(void* buf, size_t len, scheduler::Process* proc);
		const nx::string& symbolicate(addr_t addr, scheduler::Process* proc);
	}

	namespace disasm
	{
		// TODO: this assumes that we are performing this in the current address space!
		void printDisassembly(addr_t rip);
	}

	namespace util
	{
		nx::string plural(const nx::string& s, size_t count);


		// backtracing stuff
		void initKernelSymbols(BootInfo* bi);

		void printStackTrace(uint64_t rbp = 0, scheduler::Process* proc = 0);
		nx::array<addr_t> getStackFrames(uint64_t rbp = 0);

		void initDemangler();
		nx::string demangleSymbol(const nx::string& mangled);
	}
}
