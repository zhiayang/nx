// util.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "defs.h"

namespace nx
{
	namespace util
	{
		nx::string plural(const nx::string& s, size_t count);


		// backtracing stuff
		void initSymbols(BootInfo* bi);

		nx::string getSymbolAtAddr(addr_t addr);
		nx::array<addr_t> getStackFrames(uint64_t rbp = 0);

		void printStackTrace(uint64_t rbp = 0);


		void initDemangler();
		nx::string demangleSymbol(const nx::string& mangled);
	}
}