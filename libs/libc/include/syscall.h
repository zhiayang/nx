// syscall.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <nx/syscall.h>

#if defined(__x86_64__)

	#include <arch/x86_64/syscall_macros.h>

#else

	#error "unsupported architecture!"

#endif
