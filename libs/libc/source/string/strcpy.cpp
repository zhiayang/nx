// strcpy.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.


#include "../../include/string.h"
extern "C" char* strcpy(char* destination, const char* source)
{
	return (char*) memcpy(destination, source, strlen(source) + 1);
}

