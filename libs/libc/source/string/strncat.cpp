// strncat.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

extern "C"

#include "../../include/string.h"
extern "C" char* strncat(char* dest, const char* src, size_t n)
{
	size_t dest_len = strlen(dest);

	size_t i = 0;
	for(; i < n && src[i] != '\0'; i++)
		dest[dest_len + i] = src[i];

	dest[dest_len + i] = '\0';
	return dest;
}
