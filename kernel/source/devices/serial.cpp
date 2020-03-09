// serial.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "devices/ports.h"
#include "devices/serial.h"


#include <stdarg.h>

namespace nx {
namespace serial
{
	constexpr uint16_t COM1_SERIAL_PORT          = 0x3F8;
	constexpr uint16_t QEMU_DEBUG_SERIAL_PORT    = 0xE9;

	void debugprint(char c)
	{
		nx::port::write1b(COM1_SERIAL_PORT, c);
		nx::port::write1b(QEMU_DEBUG_SERIAL_PORT, c);
	}

	void debugprint(char* s, size_t l)
	{
		if(!s)
			return;

		for(size_t i = 0; i < l && s[i]; i++)
			debugprint(s[i]);
	}

	void debugprint(const char* s)
	{
		debugprint((char*) s, strlen(s));
	}


	void debugprintf(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);

		vcbprintf(nullptr, [](void* ctx, const char* s, size_t len) -> size_t {
			serial::debugprint((char*) s, len);
			return len;
		}, fmt, args);

		va_end(args);
	}
}
}













