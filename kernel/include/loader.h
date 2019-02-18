// loader.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

//? not entirely sure if 'loader' is the best name for this, but
//? atm i can't come up with something more descriptive.

#pragma once

#include "defs.h"

namespace nx
{
	namespace loader
	{
		bool loadELFBinary(scheduler::Process* proc, void* ptr, size_t len, addr_t* entry);

		bool loadIndeterminateBinary(scheduler::Process* proc, void* ptr, size_t len, addr_t* entry);
		scheduler::Thread* loadProgram(const nx::string& path);
	}
}
