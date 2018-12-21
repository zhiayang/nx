// pthread_misc.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <pthread.h>
#include "../../include/orionx/syscall.h"

extern "C" pthread_t pthread_self()
{
	return Library::SystemCall::GetTID();
}
