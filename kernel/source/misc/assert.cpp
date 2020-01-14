// assert.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx
{
	void assert_fail(const char* file, size_t line, const char* thing, const char* fmt, ...)
	{
		interrupts::disable();

		va_list args; va_start(args, fmt);
		abort_nohalt("failed assertion\n");
		print("invariant: %s\nreason:    ", thing);
		vprint(fmt, args);
		print("\nlocation:  %s:%zu\n\n", file, line);

		util::printStackTrace();

		halt();
	}

	void assert_fail(const char* file, size_t line, const char* thing)
	{
		assert_fail(file, line, thing, thing, "(none)");
	}
}
