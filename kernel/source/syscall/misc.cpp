// misc.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace syscall
{
	void sc_exit(int status)
	{
		// ok good. this will yield.
		scheduler::exit(status);
	}
}
}
