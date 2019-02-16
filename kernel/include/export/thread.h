// thread.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

struct thread_t
{
	thread_t* self;

	pid_t threadId;
	pid_t processId;

	size_t pendingIPCMsgCount;

};

#ifndef __cplusplus

typedef struct thread_t thread_t;

#endif
