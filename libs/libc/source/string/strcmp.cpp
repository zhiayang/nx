// strcmp.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "../../include/string.h"
extern "C" int strcmp(const char* str1, const char* str2)
{
	size_t len1 = strlen(str1);
	size_t len2 = strlen(str2);

	int cmpResult = memcmp(str1, str2, (len1 < len2) ? len1 : len2);
	if(cmpResult != 0)
		return cmpResult;

	if(len1 > len2)
		return 1;
	else if(len1 < len2)
		return -1;

	return 0;
}
