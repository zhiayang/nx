// strtok.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "../../include/string.h"
extern "C" char* strtok(char* s1, const char* s2)
{
	static char* prevS1;
	if(s1 == NULL)
	{
		if(prevS1 == NULL)
			return NULL;
		s1 = prevS1;
	}

	s1 += strspn(s1, s2);
	if(*s1 == '\0')
		return NULL;
	size_t s1newLen = strcspn(s1, s2);

	if(s1[s1newLen] == '\0')
	{
		// we are at the end of the original string
		// next call should return NULL whatever s2 is
		prevS1 = NULL;
	}
	else
	{
		s1[s1newLen] = '\0';
		prevS1 = s1 + s1newLen + 1;
	}

	return s1;
}
