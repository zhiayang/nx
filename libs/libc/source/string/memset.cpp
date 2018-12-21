// memset.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stddef.h>
extern "C" void* memset(void* ptr, int value, size_t num)
{
	// 'stosl' will use the value in eax but we only want the value in al
	// so we make eax = al << 24 | al << 16 | al << 8 | al
	if((value & 0xff) == 0)
		// this is a little optimization because memset is most often called with value = 0
		value = 0;

	else
	{
		value = (value & 0xff) | ((value & 0xff) << 8);
		value = (value & 0xffff) | ((value & 0xffff) << 16);
	}

	void* temporaryPtr = ptr;
	asm volatile("rep stosl ; mov %3, %2 ; rep stosb" : "+D"(temporaryPtr) : "a"(value), "c"(num / 4), "r"(num % 4) : "cc", "memory");
	return ptr;
}
