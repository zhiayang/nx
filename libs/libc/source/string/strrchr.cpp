// strrchr.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "../../include/string.h"
extern "C" char* strrchr(const char* str, int character)
{
	int i = (int) strlen(str);
	for(; i >= 0; i--)
		if(str[i] == character)
			return (char*) str + i;

	return NULL;
}


