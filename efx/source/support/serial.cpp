// serial.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <stddef.h>

namespace serial
{
	static void write1b(uint8_t value, uint16_t port)
	{
		asm volatile("outb %0, %1" :: "a"(value), "Nd"(port));
	}

	void print(char* s, size_t len)
	{
		for(size_t i = 0; i < len; i++)
			write1b(s[i], 0xE9);
	}

	void print(char* s)
	{
		while(s && *s)
			write1b(*s++, 0xE9);
	}
}
