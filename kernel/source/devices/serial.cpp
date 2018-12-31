// serial.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "devices/ports.h"
#include "devices/serial.h"

#include "string.h"

namespace nx {
namespace serial
{
	static constexpr uint16_t QEMU_DEBUG_SERIAL_PORT = 0xE9;

	void debugprint(char c)
	{
		nx::port::write1b(c, QEMU_DEBUG_SERIAL_PORT);
	}

	void debugprint(char* s, size_t l)
	{
		for(size_t i = 0; i < l; i++)
			debugprint(s[i]);
	}

	void debugprint(const char* s)
	{
		debugprint((char*) s, strlen(s));
	}
}
}