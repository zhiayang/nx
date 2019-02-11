// misc.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace syscall
{
	int64_t sc_exit(int status)
	{
		// ok good. this will yield.
		scheduler::exit(status);
	}
}
}
