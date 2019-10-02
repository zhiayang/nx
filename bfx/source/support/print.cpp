// print.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdarg.h>
#include <string.h>

#include "krt.h"
#include "bfx.h"


namespace bfx
{
	static size_t cb_print(void* ctx, const char* s, size_t len)
	{
		serial::print((char*) s, len);
		for(size_t i = 0; i < len; i++)
			console::putchar(s[i]);

		return len;
	}

	static int vprint(const char* fmt, va_list args)
	{
		return vcbprintf(nullptr, cb_print, fmt, args);
	}

	static void vabort(const char* fmt, va_list args)
	{
		print("error: ");
		vprint(fmt, args);
		print("\nan error occured while loading the kernel; halting now\n\n");
	}

	void abort(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);
		vabort(fmt, args);
		va_end(args);

		while(1);
	}

	int print(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);

		int ret = vprint(fmt, args);

		va_end(args);
		return ret;
	}

	int println(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);

		int ret = vprint(fmt, args);
		ret += cb_print(nullptr, "\n", 1);

		va_end(args);
		return ret;
	}
}

























