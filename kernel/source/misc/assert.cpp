// assert.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx
{
	void assert_fail(const char* file, size_t line, const char* thing)
	{
		abort("failed assertion\ninvariant: %s\nlocation: %s:%zu\n\n", thing, file, line);
	}
}