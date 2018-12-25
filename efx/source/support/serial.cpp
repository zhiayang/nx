// serial.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "efx.h"
#include "krt.h"

namespace serial
{
	constexpr uint16_t COM1_PORT = 0x3F8;

	void print(char* s, size_t len)
	{
		for(size_t i = 0; i < len; i++)
			krt::port::write1b(s[i], 0xE9);
	}
}
