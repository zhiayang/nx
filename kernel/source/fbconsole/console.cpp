// console.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace console
{
	static bool Initialised = false;


	void putchar(char c)
	{
		if(!Initialised)
		{
			fallback::putchar(c);
		}
		else
		{
		}
	}
}
}
