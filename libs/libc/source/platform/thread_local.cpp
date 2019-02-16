// thread_local.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "../../include/nx/thread_local.h"

extern "C" pid_t getThreadId()  { return getCurrentThread()->threadId; }
extern "C" pid_t getProcessId() { return getCurrentProcess()->processId; }


extern "C" thread_t* getCurrentThread()
{
	thread_t* ret = 0;
	asm volatile ("movq %%fs:0, %0" : "=r"(ret));

	return ret;
}

extern "C" process_t* getCurrentProcess()
{
	return getCurrentThread()->parentProcess;
}
