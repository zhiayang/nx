// misc.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#include "../../include/orionx/syscall.h"
#include <sys/types.h>

extern "C" int usleep(useconds_t usec)
{
	Library::SystemCall::Sleep((usec + 999) / 1000);
	return 0;
}

extern "C" pid_t fork()
{
	return Library::SystemCall::ForkProcess();
}
