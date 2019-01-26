// thread.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace scheduler
{
	static constexpr size_t KernelStackSize = 0x4000;

	Thread* createThread(Process* p, Fn0Args_t fn)
	{
		return createThread(p, (Fn6Args_t) fn, 0, 0, 0, 0, 0, 0);
	}
	Thread* createThread(Process* p, Fn1Arg_t fn, void* a)
	{
		return createThread(p, (Fn6Args_t) fn, a, 0, 0, 0, 0, 0);
	}
	Thread* createThread(Process* p, Fn2Args_t fn, void* a, void* b)
	{
		return createThread(p, (Fn6Args_t) fn, a, b, 0, 0, 0, 0);
	}
	Thread* createThread(Process* p, Fn3Args_t fn, void* a, void* b, void* c)
	{
		return createThread(p, (Fn6Args_t) fn, a, b, c, 0, 0, 0);
	}
	Thread* createThread(Process* p, Fn4Args_t fn, void* a, void* b, void* c, void* d)
	{
		return createThread(p, (Fn6Args_t) fn, a, b, c, d, 0, 0);
	}
	Thread* createThread(Process* p, Fn5Args_t fn, void* a, void* b, void* c, void* d, void* e)
	{
		return createThread(p, (Fn6Args_t) fn, a, b, c, d, e, 0);
	}


	Thread* createThread(Process* p, Fn6Args_t fn, void* a, void* b, void* c, void* d, void* e, void* f)
	{
		// setup the kernel stack

	}
}
}



















