// debugcon.cpp
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace debugcon
{
	static void rx_handler(uint8_t c)
	{
		serial::debugprint(c);
	}

	void init()
	{
		serial::initReceive(rx_handler);
	}
}
}
