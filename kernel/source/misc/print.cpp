// print.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdarg.h>

#include "nx.h"
#include "krt.h"
#include "string.h"

namespace nx
{
	bool ENABLE_DEBUG = true;

	void console_print(const char* s, size_t len)
	{
		for(size_t i = 0; i < len; i++)
			console::putchar(s[i]);

		// mirror to the serial port as well.
		return serial::debugprint(s, len);
	}

	[[noreturn]] void halt()
	{
		serial::debugprintf("\nhalting...\n\n");

		asm volatile("cli");
		while(true) asm volatile("hlt");
	}

	[[noreturn]] void __internal_abort()
	{
		asm volatile("cli");

		serial::debugprintf("\n");
		util::printStackTrace();

		halt();
	}
}

























