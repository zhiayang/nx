// krt.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include <stdarg.h>

#include "stdint.h"
#include "krt/meta.h"
#include "krt/types.h"
#include "krt/macros.h"

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


int vcbprintf(void* ctx, size_t (*callback)(void*, const char*, size_t), const char* format, va_list parameters);
int print(const char* fmt, ...);


__END_DECLS

















