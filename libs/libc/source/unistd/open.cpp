// open.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#if 0

#include "../../include/fcntl.h"
#include <stdarg.h>
#include "../../include/orionx/syscall.h"

extern "C" int open(const char* path, int flags, ...)
{
	mode_t mode = 0;
	(void) mode;
	if(flags & O_CREAT)
	{
		va_list ap;
		va_start(ap, flags);
		mode = va_arg(ap, mode_t);
		va_end(ap);
	}

	// TODO: flags ignored for now
	return (int) Library::SystemCall::Open(path, flags);
}

#endif
