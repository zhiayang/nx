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
#include "krt/types/array.h"
#include "krt/types/stack.h"

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

	namespace heap
	{
		void init();

		void* alloc(size_t sz);
		void free(void* ptr);
		void* realloc(void* ptr, size_t newsz);
	}

	namespace util
	{
		size_t humanSizedBytes(char* buffer, size_t bytes, bool thousand = false);

		template <typename allocator, typename aborter>
		krt::string<allocator, aborter> humanSizedBytes(size_t bytes, bool thousand = false)
		{
			size_t len = 0; char buffer[256];
			len = humanSizedBytes(&buffer[0], bytes, thousand);
			return krt::string<allocator, aborter>(buffer, len);
		}

		void memfill1b(uint8_t* ptr, uint8_t val, size_t count);
		void memfill2b(uint16_t* ptr, uint16_t val, size_t count);
		void memfill4b(uint32_t* ptr, uint32_t val, size_t count);
		void memfill8b(uint64_t* ptr, uint64_t val, size_t count);
	}
}




















// c stuff!
__BEGIN_DECLS

int atoi(const char* nptr);
long atol(const char* nptr);
double atof(const char* nptr);
long long atoll(const char* nptr);

float strtof(const char* nptr, char** endptr);
double strtod(const char* nptr, char** endptr);
long double strtold(const char* nptr, char** endptr);

long strtol(const char* nptr, char** endptr, int base);
long long strtoll(const char* nptr, char** endptr, int base);
unsigned long strtoul(const char* nptr, char** endptr, int base);
unsigned long long strtoull(const char* nptr, char** endptr, int base);

int abs(int j);
long int labs(long int j);
long long llabs(long long int j);

char* itoa(int num, char* dest, int base);
char* ltoa(long num, char* dest, int base);
char* lltoa(long long num, char* dest, int base);


int cbprintf(void* ctx, size_t (*callback)(void*, const char*, size_t), const char* format, ...);
int vcbprintf(void* ctx, size_t (*callback)(void*, const char*, size_t), const char* format, va_list parameters);


__END_DECLS

















