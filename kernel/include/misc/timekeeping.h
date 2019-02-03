// timekeeping.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include "defs.h"

namespace nx
{
	namespace time
	{
		constexpr uint64_t NS_PER_S  = 1'000'000'000ULL;
		constexpr uint64_t US_PER_S  = 1'000'000ULL;
		constexpr uint64_t MS_PER_S  = 1'000ULL;

		// record in nanoseconds.
		template <typename T = uint64_t>
		struct unit
		{
			constexpr unit() : _ns(0)       { }
			constexpr unit(T v) : _ns(v)    { }

			constexpr T s()     { return _ns / (NS_PER_S); }
			constexpr T ms()    { return _ns / (NS_PER_S / MS_PER_S); }
			constexpr T us()    { return _ns / (NS_PER_S / US_PER_S); }
			constexpr T ns()    { return _ns / (NS_PER_S / NS_PER_S); }

			private:
			T _ns;
		};

		constexpr unit<uint64_t> seconds(uint64_t s)        { return unit(s * (NS_PER_S)); }
		constexpr unit<uint64_t> milliseconds(uint64_t s)   { return unit(s * (NS_PER_S / MS_PER_S)); }
		constexpr unit<uint64_t> microseconds(uint64_t s)   { return unit(s * (NS_PER_S / US_PER_S)); }
		constexpr unit<uint64_t> nanoseconds(uint64_t s)    { return unit(s * (NS_PER_S / NS_PER_S)); }
	}
}