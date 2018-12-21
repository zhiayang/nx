// perror.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/stdio.h"


void perror(const char* msg)
{
	// TODO: inspect errno
	// for now:

	fprintf(stderr, "%s: unknown error.\n", msg ? msg : "application");
}
