// strncpy.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "../../include/string.h"

extern "C" char* strncpy(char* destination, const char* source, size_t n)
{
	size_t len = strlen(source) + 1;
	if(len > n) len = n;
	memcpy(destination, source, len);
	if(len < n) memset(destination + len, '\0', n - len);
	return destination;
}

