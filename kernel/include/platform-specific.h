// platform-specific.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>

namespace nx
{
	enum class Platform         { PC, RPi3 };
	enum class Architecture     { x64, AArch64 };

	#ifdef __ARCH_x64__

		constexpr Platform getPlatform()            { return Platform::PC; }
		constexpr Architecture getArchitecture()    { return Architecture::x64; }

		constexpr int IRQ_BASE_VECTOR               = 32;


		constexpr bool IS_CANONICAL(uintptr_t x)
		{
			return (x >= 0xFFFF'8000'0000'0000) || x <= (0x7FFF'FFFF'FFFF);
		}

	#else

	#endif
}


