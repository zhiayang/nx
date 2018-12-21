// strstr.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "../../include/string.h"
extern "C" char* strstr(const char* s1, const char* s2)
{
	size_t s2len = strlen(s2);
	for(; *s1 != '\0'; s1++)
	{
		int i;
		for(i = 0; i < (int) s2len; i++)
		{
			if(s2[i] == '\0')
				return (char*) s1;
			if(s2[i] == *s1)
				continue;
			break;
		}
	}
	return NULL;
}

