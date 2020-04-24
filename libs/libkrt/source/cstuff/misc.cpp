// misc.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "krt.h"

int abs(int j)
{
	return (j < 0) ? (-j) : j;
}

long int labs(long int j)
{
	return (j < 0) ? (-j) : j;
}

long long int llabs(long long int j)
{
	return (j < 0) ? (-j) : j;
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





