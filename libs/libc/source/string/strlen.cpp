// strlen.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.



#include <stddef.h>
extern "C" size_t strlen(const char* str)
{
	size_t len = 0;
	const char* endPtr = str;
	asm("repne scasb" : "+D"(endPtr) : "a"(0), "c"(~0) : "cc");
	len = (endPtr - str) - 1;
	return len;
}

