// defs.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

#include "krt.h"

using addr_t = uintptr_t;

namespace nx
{
	struct _allocator
	{
		static void* allocate(size_t sz, size_t align);
		static void deallocate(void* pt);
	};

	struct _aborter
	{
		static void abort(const char* fmt, ...);
		static void debuglog(const char* fmt, ...);
	};


	// re-export the krt types with our own stuff.
	using string = krt::string<_allocator, _aborter>;

	template<typename T> using array = krt::array<T, _allocator, _aborter>;
	template<typename T> using stack = krt::stack<T, _allocator, _aborter>;

	[[noreturn]] void vabort(const char* fmt, va_list args);
	[[noreturn]] void abort(const char* fmt, ...);

	[[noreturn]] void assert_fail(const char* file, size_t line, const char* thing);
}

#ifdef assert
#undef assert
#endif


#define assert(x)   ((x) ? ((void) 0) : ::nx::assert_fail(__FILE__, __LINE__, #x))








