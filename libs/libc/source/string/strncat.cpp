// strncat.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

extern "C"

#include "../../include/string.h"
extern "C" char* strncat(char* s1, const char* s2, size_t n)
{
	size_t n2 = strlen(s2);
	if(n2 > n) n2 = n;
	strncpy(s1 + strlen(s1), s2, n2);
	s1[strlen(s1) + n2] = '\0';
	return s1;
}
