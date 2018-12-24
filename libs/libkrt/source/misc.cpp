// misc.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "krt.h"

extern "C" int abs(int j)
{
	return (j < 0) ? (-j) : j;
}

extern "C" long int labs(long int j)
{
	return (j < 0) ? (-j) : j;
}

extern "C" long long int llabs(long long int j)
{
	return (j < 0) ? (-j) : j;
}

extern "C" long long atoll(const char* str)
{
	return strtoll(str, (char**) NULL, 10);
}

extern "C" long atol(const char* str)
{
	return strtol(str, (char**) NULL, 10);
}

extern "C" int atoi(const char* str)
{
	return (int) strtol(str, (char**) NULL, 10);
}
extern "C" double atof(const char* str)
{
	return strtod(str, NULL);
}



static size_t convert_integer(char* destination, uintmax_t value, uintmax_t base, const char* digits)
{
	size_t result = 1;
	uintmax_t copy = value;
	while(base <= copy)
		copy /= base, result++;

	for(size_t i = result; i != 0; i--)
	{
		destination[i - 1] = digits[value % base];
		value /= base;
	}
	return result;
}

extern "C" char* itoa(int num, char* dest, int base)
{
	convert_integer(dest, num, base, "0123456789abcdefghijklmnopqrstuvwxyz");
	return dest;
}

extern "C" char* ltoa(long num, char* dest, int base)
{
	convert_integer(dest, num, base, "0123456789abcdefghijklmnopqrstuvwxyz");
	return dest;
}

extern "C" char* lltoa(long long num, char* dest, int base)
{
	convert_integer(dest, num, base, "0123456789abcdefghijklmnopqrstuvwxyz");
	return dest;
}





