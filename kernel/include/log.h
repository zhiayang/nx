// log.h
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "defs.h"

namespace nx
{
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

	constexpr const char* DBG_COLOUR = (USE_COLOURS ? COLOUR_WHITE : "");
	constexpr const char* LOG_COLOUR = (USE_COLOURS ? COLOUR_GREY_BOLD : "");
	constexpr const char* WRN_COLOUR = (USE_COLOURS ? COLOUR_YELLOW_BOLD : "");
	constexpr const char* ERR_COLOUR = (USE_COLOURS ? COLOUR_RED_BOLD : "");

	// so we can toggle this without recompiling the whole thing, put it in
	// a cpp file.
	extern bool ENABLE_DEBUG;

	template <typename... Args>
	void __generic_log(int lvl, const char* sys, const char* fmt, Args&&... args)
	{
		if(!ENABLE_DEBUG && lvl < 0)
			return;

		interrupts::disable();
		size_t margin = 0;

		auto cb = [&margin](const char* s, size_t len) {
			for(size_t i = 0; i < len; i++)
			{
				serial::debugprintchar(s[i]);
				if(s[i] == '\n')
				{
					// print some padding.
					for(size_t k = 0; k < margin; k++)
						serial::debugprintchar(' ');
				}
			}
		};

		const char* col = 0;
		const char* str = 0;

		if(lvl == -1)       { col = DBG_COLOUR;  str = "[dbg]"; }
		else if(lvl == 0)   { col = LOG_COLOUR;  str = "[log]"; }
		else if(lvl == 1)   { col = WRN_COLOUR;  str = "[wrn]"; }
		else if(lvl == 2)   { col = ERR_COLOUR;  str = "[err]"; }

		zpr::cprint(cb, "{}{}{} {}{}{}: ", col, str, RESET_COLOUR, SUBSYS_COLOUR, sys, RESET_COLOUR);

		margin = 5 + 1 + strlen(sys) + 2;

		zpr::cprint(cb, fmt, static_cast<Args&&>(args)...);

		margin = 0;
		zpr::cprint(cb, "\n");

		interrupts::enable();
	}

	template <typename... Args>
	void dbg(const char* sys, const char* fmt, Args&&... args)
	{
		__generic_log(-1, sys, fmt, static_cast<Args&&>(args)...);
	}

	template <typename... Args>
	void log(const char* sys, const char* fmt, Args&&... args)
	{
		__generic_log(0, sys, fmt, static_cast<Args&&>(args)...);
	}

	template <typename... Args>
	void warn(const char* sys, const char* fmt, Args&&... args)
	{
		__generic_log(1, sys, fmt, static_cast<Args&&>(args)...);
	}

	template <typename... Args>
	void error(const char* sys, const char* fmt, Args&&... args)
	{
		__generic_log(2, sys, fmt, static_cast<Args&&>(args)...);
	}
}


template <>
struct zpr::print_formatter<nx::PhysAddr>
{
	template <typename Cb>
	void print(nx::PhysAddr addr, Cb&& cb, format_args args)
	{
		cb("p:");
		args.specifier = 'p';
		print_formatter<addr_t>().print(addr.addr(), static_cast<Cb&&>(cb),
			static_cast<format_args&&>(args));
	}
};

template <>
struct zpr::print_formatter<nx::VirtAddr>
{
	template <typename Cb>
	void print(nx::VirtAddr addr, Cb&& cb, format_args args)
	{
		cb("v:");
		args.specifier = 'p';
		print_formatter<addr_t>().print(addr.addr(), static_cast<Cb&&>(cb),
			static_cast<format_args&&>(args));
	}
};
