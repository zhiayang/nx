// strcat.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/string.h"
extern "C" char* strcat(char* s1, const char* s2)
{
	strcpy(s1 + strlen(s1), s2);
	return s1;
}
