// itoa.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/stdio.h"
#include "../../include/defs/_printf.h"

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

