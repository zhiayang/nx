// main.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <stddef.h>

extern "C" int main()
{
	uint32_t* fb = (uint32_t*) 0xFFFF'FFFF'0000'0000;
	for(int y = 0; y < 300; y++)
	{
		for(int x = 0; x < 800; x++)
		{
			*(fb + y * 800 + x) = 0xffff0000;
		}
	}

	while(1);
}
