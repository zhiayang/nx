// print.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdarg.h>

#include "nx.h"
#include "krt.h"
#include "string.h"

namespace nx
{
	static size_t cb_serialprint(void* ctx, const char* s, size_t len)
	{
		serial::debugprint((char*) s, len);
		return len;
	}

	static size_t cb_print(void* ctx, const char* s, size_t len)
	{
		for(size_t i = 0; i < len; i++)
			console::putchar(s[i]);

		// mirror to the serial port as well.
		return cb_serialprint(ctx, s, len);
	}

	static size_t cb_sprint(void* ctx, const char* s, size_t len)
	{
		// ctx is actually a pointer to nx::string ok
		auto str = (nx::string*) ctx;
		str->append(nx::string(s, len));

		return len;
	}

	int vprint(const char* fmt, va_list args)
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







	void log(const char* sys, const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);
		cbprintf(nullptr, cb_serialprint, "[d] %s: ", sys);
		vcbprintf(nullptr, cb_serialprint, fmt, args);
		cbprintf(nullptr, cb_serialprint, "\n");
		va_end(args);
	}


	void warn(const char* sys, const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);
		cbprintf(nullptr, cb_serialprint, "[w] %s: ", sys);
		vcbprintf(nullptr, cb_serialprint, fmt, args);
		cbprintf(nullptr, cb_serialprint, "\n");
		va_end(args);
	}

	void error(const char* sys, const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);
		cbprintf(nullptr, cb_serialprint, "[e] %s: ", sys);
		vcbprintf(nullptr, cb_serialprint, fmt, args);
		cbprintf(nullptr, cb_serialprint, "\n");
		va_end(args);
	}















	[[noreturn]] void halt()
	{
		serial::debugprintf("\nhalting...");

		asm volatile ("cli");
		while(true) asm volatile ("hlt");
	}

	void vabort_nohalt(const char* fmt, va_list args)
	{
		serial::debugprintf("\n\nkernel abort! error: ");
		vprint(fmt, args);
		va_end(args);
	}

	void abort_nohalt(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);
		vabort_nohalt(fmt, args);
	}



	[[noreturn]] void vabort(const char* fmt, va_list args)
	{
		vabort_nohalt(fmt, args);
		util::printStackTrace();

		while(true);
	}

	[[noreturn]] void abort(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);
		vabort(fmt, args);
	}

	void _aborter::abort(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);
		vabort(fmt, args);
		va_end(args);
	}

	void _aborter::debuglog(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);
		vprint(fmt, args);
		va_end(args);
	}
}

























