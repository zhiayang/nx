// pthread_create.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#include <pthread.h>
#include "../../include/orionx/syscall.h"
#include <string.h>

extern "C" int pthread_create(pthread_t* restrict thread, const pthread_attr_t* restrict oattr, void *(*start_routine)(void*), void* restrict arg)
{
	// thread creation should usually work, but we have to check the return value just in case.
	pthread_attr_t sattr;
	memset(&sattr, 0, sizeof(pthread_attr_t));
	if(oattr == NULL)
	{
		sattr.a1 = arg;
	}
	else
	{
		memcpy(&sattr, oattr, sizeof(pthread_attr_t));
		sattr.a1 = arg;
	}

	uint64_t tid = Library::SystemCall::CreateThread(&sattr, (void(*)()) start_routine);
	*thread = tid;

	return tid > 0 ? 0 : -1;
}
