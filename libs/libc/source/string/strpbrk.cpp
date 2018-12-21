// strpbrk.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "../../include/string.h"
extern "C" char* strpbrk(const char* s1, const char* s2)
{
	int s2len = (int) strlen(s2);
	for(; *s1 != '\0'; s1++)
	{
		int i;
		for(i = 0; i < s2len; i++)
			if(*s1 == s2[i])
				return (char*) s1;
	}
	return NULL;
}
