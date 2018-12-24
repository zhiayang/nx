// getpid.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#include "../../include/unistd.h"
#include "../../include/orionx/syscall.h"

extern "C" pid_t getpid()
{
	return (pid_t) Library::SystemCall::GetPID();
}

extern "C" pid_t getppid()
{
	return (pid_t) Library::SystemCall::GetParentPID();
}
