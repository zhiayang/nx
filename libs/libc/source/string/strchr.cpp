// strchr.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "../../include/string.h"
extern "C" char* strchr(const char* s, int c)
{
	return (char*) memchr(s, c, strlen(s) + 1);
}

