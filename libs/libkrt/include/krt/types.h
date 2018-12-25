// types.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"
#include "string.h"

#include "krt/array.h"

namespace krt
{
	struct kernel_allocator
	{
		static void* allocate(size_t sz);
		static void deallocate(void* pt);
	};

	struct kernel_aborter
	{
		static void abort(const char* fmt, ...);
		static void debuglog(const char* fmt, ...);
	};

	template <typename allocator = kernel_allocator, typename aborter = kernel_aborter>
	struct string : array<char, allocator, aborter>
	{
		using array<char, allocator, aborter>::array;

		string() : string(nullptr, 0) { }
		string(const char p[]) : string((char*) p, strlen(p)) { }
	};
}














