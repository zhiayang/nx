// strspn.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "../../include/string.h"
extern "C" size_t strspn(const char* s1, const char* s2)
{
	int found = 0;
	size_t len = 0;
	do {
		int i;
		for(i = 0; i < (int) strlen(s2); i++)
		{
			if(s1[len] == s2[i])
			{
				len++;
				found = 1;
				break;
			}
		}
	} while(found);

	return len;
}
