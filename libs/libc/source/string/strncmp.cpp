// strncmp.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "../../include/string.h"
extern "C" int strncmp(const char* s1, const char* s2, size_t n)
{
	return memcmp(s1, s2, n);
}
