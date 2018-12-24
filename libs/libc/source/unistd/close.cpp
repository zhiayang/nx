// close.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#include "../../include/unistd.h"
#include "../../include/orionx/syscall.h"

extern "C" int close(int fd)
{
	Library::SystemCall::Close(fd);
	return 0;
}
