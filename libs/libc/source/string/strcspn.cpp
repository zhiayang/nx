// strcspn.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "../../include/string.h"
extern "C" size_t strcspn(const char* s1, const char* s2)
{
	char* pbrk = strpbrk(s1, s2);
	if(pbrk == NULL)
		return strlen(s1);

	return (size_t)(pbrk - s1);
}

