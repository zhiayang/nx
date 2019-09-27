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
		// mirror to the serial port as well.
		serial::print((char*) s, len);

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




	constexpr size_t BUFFER_LEN = 512;
	static char c8_buffer[BUFFER_LEN];
	static char16_t c16_buffer[BUFFER_LEN];

	char16_t* convertstr(char* inp, size_t len)
	{
		size_t i = 0;
		for(; i < __min(len, BUFFER_LEN) && *inp; i++, inp++)
		{
			if(*inp == '\n')
			{
				// add \r for them
				c16_buffer[i] = '\r';
				c16_buffer[++i] = '\n';
			}
			else
			{
				c16_buffer[i] = *inp;
			}
		}

		c16_buffer[i] = 0;
		return &c16_buffer[0];
	}

	char* convertstr(char16_t* inp, size_t len)
	{
		size_t i = 0;
		for(; i < __min(len, BUFFER_LEN) && *inp; i++, inp++)
		{
			if(*inp == '\r')
			{
				// skip carriage return
			}
			else
			{
				c8_buffer[i] = (char) *inp;
			}
		}

		c8_buffer[i] = 0;
		return &c8_buffer[0];
	}



	bfx::string humanSizedBytes(size_t bytes, bool thou)
	{
		return krt::util::humanSizedBytes<allocator, aborter>(bytes, thou);
	}

	static size_t cb_sprint(void* ctx, const char* s, size_t len)
	{
		// ctx is actually a pointer to efx::string ok
		auto str = (bfx::string*) ctx;
		str->append(bfx::string(s, len));

		return len;
	}

	bfx::string sprint(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);

		bfx::string ret;
		vcbprintf(&ret, cb_sprint, fmt, args);

		va_end(args);
		return ret;
	}




	void aborter::abort(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);
		vabort(fmt, args);

		va_end(args);
	}

	void aborter::debuglog(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);
		vprint(fmt, args);

		va_end(args);
	}
}

























