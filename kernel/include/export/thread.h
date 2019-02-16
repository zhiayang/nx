// thread.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>


struct process_t
{
	pid_t processId;
};

struct thread_t
{
	thread_t* self;

	pid_t threadId;
	process_t* parentProcess;
};

#ifndef __cplusplus

typedef struct thread_t thread_t;
typedef struct process_t process_t;

#endif
