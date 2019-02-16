// thread_local.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <nx/thread.h>
#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

thread_t* getCurrentThread();
process_t* getCurrentProcess();

pid_t getThreadId();
pid_t getProcessId();

__END_DECLS
