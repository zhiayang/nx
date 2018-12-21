// assert.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/assert.h"
#include "../../include/stdio.h"
#include "../../include/stdlib.h"

extern "C" void __assert(const char* filename, unsigned long line, const char* function_name, const char* expression)
{
	fprintf(stderr, "Assertion failure: %s:%lu: %s: %s\n", filename, line,
	        function_name, expression);

	abort();
}
