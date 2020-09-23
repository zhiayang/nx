// krt.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include <stdarg.h>

#include "stdint.h"
#include "krt/meta.h"
#include "krt/macros.h"

// include the types.
#include "krt/types/str.h"
#include "krt/types/list.h"
#include "krt/types/array.h"
#include "krt/types/stack.h"
#include "krt/types/treemap.h"
#include "krt/types/circbuf.h"
#include "krt/types/hashmap.h"
#include "krt/types/hashset.h"
#include "krt/types/optional.h"
#include "krt/types/string_view.h"

namespace krt
{
	namespace port
	{
		uint8_t read1b(uint16_t port);
		uint16_t read2b(uint16_t port);
		uint32_t read4b(uint16_t port);

		void write1b(uint8_t value, uint16_t port);
		void write2b(uint16_t value, uint16_t port);
		void write4b(uint32_t value, uint16_t port);
	}

	namespace util
	{
		void memfill1b(uint8_t* ptr, uint8_t val, size_t count);
		void memfill2b(uint16_t* ptr, uint16_t val, size_t count);
		void memfill4b(uint32_t* ptr, uint32_t val, size_t count);
		void memfill8b(uint64_t* ptr, uint64_t val, size_t count);
	}
}















extern "C" {
	int atoi(const char* nptr);

	long strtol(const char* nptr, char** endptr, int base);
	long long strtoll(const char* nptr, char** endptr, int base);
	unsigned long strtoul(const char* nptr, char** endptr, int base);
	unsigned long long strtoull(const char* nptr, char** endptr, int base);

	int abs(int j);
	long int labs(long int j);
	long long llabs(long long int j);

	int snprintf(char* str, size_t size, const char* fmt, ...);
	int cbprintf(void* ctx, size_t (*callback)(void*, const char*, size_t), const char* format, ...);
	int vcbprintf(void* ctx, size_t (*callback)(void*, const char*, size_t), const char* format, va_list parameters);
}


















