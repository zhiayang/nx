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





	constexpr const char* COLOUR_RESET          = "\033[0m";
	constexpr const char* COLOUR_BLACK          = "\033[30m";
	constexpr const char* COLOUR_RED            = "\033[31m";
	constexpr const char* COLOUR_GREEN          = "\033[32m";
	constexpr const char* COLOUR_YELLOW         = "\033[33m";
	constexpr const char* COLOUR_BLUE           = "\033[34m";
	constexpr const char* COLOUR_MAGENTA        = "\033[35m";
	constexpr const char* COLOUR_CYAN           = "\033[36m";
	constexpr const char* COLOUR_WHITE          = "\033[37m";
	constexpr const char* COLOUR_BLACK_BOLD     = "\033[1m";
	constexpr const char* COLOUR_RED_BOLD       = "\033[1m\033[31m";
	constexpr const char* COLOUR_GREEN_BOLD     = "\033[1m\033[32m";
	constexpr const char* COLOUR_YELLOW_BOLD    = "\033[1m\033[33m";
	constexpr const char* COLOUR_BLUE_BOLD      = "\033[1m\033[34m";
	constexpr const char* COLOUR_MAGENTA_BOLD   = "\033[1m\033[35m";
	constexpr const char* COLOUR_CYAN_BOLD      = "\033[1m\033[36m";
	constexpr const char* COLOUR_WHITE_BOLD     = "\033[1m\033[37m";
	constexpr const char* COLOUR_GREY_BOLD      = "\033[30;1m";


	constexpr bool USE_COLOURS = true;
	constexpr const char* RESET_COLOUR = (USE_COLOURS ? COLOUR_RESET : "");
	constexpr const char* SUBSYS_COLOUR = (USE_COLOURS ? COLOUR_BLUE_BOLD : "");

	constexpr const char* LOG_COLOUR = (USE_COLOURS ? COLOUR_GREY_BOLD : "");
	constexpr const char* WRN_COLOUR = (USE_COLOURS ? COLOUR_YELLOW_BOLD : "");
	constexpr const char* ERR_COLOUR = (USE_COLOURS ? COLOUR_RED_BOLD : "");

	constexpr const char* LVL_COLOURS[] = { LOG_COLOUR, WRN_COLOUR, ERR_COLOUR };
	constexpr const char* LVL_STRINGS[] = { "[log]", "[wrn]", "[err]" };


	static inline void vgeneric_log(int lvl, const char* sys, const char* fmt, va_list args)
	{
		interrupts::disable();
		size_t margin = 0;

		auto cb = [](void* ctx, const char* s, size_t len) -> size_t {
			for(size_t i = 0; i < len; i++)
			{
				serial::debugprint(s[i]);
				if(s[i] == '\n')
				{
					// print some padding.
					for(size_t k = 0; k < *((size_t*) ctx); k++)
						serial::debugprint(' ');
				}
			}

			return len;
		};

		cbprintf(nullptr, cb_serialprint, "%s%s%s %s%s%s: ", LVL_COLOURS[lvl], LVL_STRINGS[lvl], RESET_COLOUR,
			SUBSYS_COLOUR, sys, RESET_COLOUR);

		margin = strlen(LVL_STRINGS[0]) + 1 + strlen(sys) + 2;

		vcbprintf(&margin, cb, fmt, args);
		cbprintf(nullptr, cb_serialprint, "\n");

		interrupts::enable();
	}

	void log(const char* sys, const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);
		vgeneric_log(0, sys, fmt, args);
		va_end(args);
	}


	void warn(const char* sys, const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);
		vgeneric_log(1, sys, fmt, args);
		va_end(args);
	}

	void error(const char* sys, const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);
		vgeneric_log(2, sys, fmt, args);
		va_end(args);
	}















	[[noreturn]] void halt()
	{
		serial::debugprintf("\nhalting...\n\n");

		asm volatile ("cli");
		while(true) asm volatile ("hlt");
	}

	void vabort_nohalt(const char* fmt, va_list args)
	{
		asm volatile ("cli");
		serial::debugprintf("\n\nkernel abort! error: ");
		vprint(fmt, args);
		va_end(args);
	}

	void abort_nohalt(const char* fmt, ...)
	{
		asm volatile ("cli");
		va_list args; va_start(args, fmt);
		vabort_nohalt(fmt, args);
	}



	[[noreturn]] void vabort(const char* fmt, va_list args)
	{
		asm volatile ("cli");
		vabort_nohalt(fmt, args);
		util::printStackTrace();

		halt();
	}

	[[noreturn]] void abort(const char* fmt, ...)
	{
		asm volatile ("cli");
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

























