// print.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdarg.h>

#include "nx.h"
#include "krt.h"
#include "string.h"

namespace nx
{
	static size_t cb_print(void* ctx, const char* s, size_t len)
	{
		// mirror to the serial port as well.
		serial::debugprint((char*) s, len);
		return len;
	}

	static size_t cb_sprint(void* ctx, const char* s, size_t len)
	{
		// ctx is actually a pointer to nx::string ok
		auto str = (nx::string*) ctx;
		str->append(nx::string(s, len));

		return len;
	}

	static int vprint(const char* fmt, va_list args)
	{
		return vcbprintf(nullptr, cb_print, fmt, args);
	}

	void print(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);

		vprint(fmt, args);

		va_end(args);
	}

	void println(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);

		vprint(fmt, args);
		cb_print(nullptr, "\n", 1);

		va_end(args);
	}

	nx::string sprint(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);

		nx::string ret;
		vcbprintf(&ret, cb_sprint, fmt, args);

		va_end(args);
		return ret;
	}









	static void vabort(const char* fmt, va_list args)
	{
		print("kernel abort! error: ");
		vprint(fmt, args);
	}

	void abort(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);
		vabort(fmt, args);
		va_end(args);

		while(true);
	}

	void _aborter::abort(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);
		vabort(fmt, args);
		va_end(args);

		while(true);
	}

	void _aborter::debuglog(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);
		vprint(fmt, args);
		va_end(args);
	}
}

























