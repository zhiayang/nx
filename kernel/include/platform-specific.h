// platform-specific.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

namespace nx
{
	enum class Platform         { PC, RPi3 };
	enum class Architecture     { x64, AArch64 };

	#ifdef __ARCH_x64__

		constexpr Platform getPlatform()            { return Platform::PC; }
		constexpr Architecture getArchitecture()    { return Architecture::x64; }


		constexpr int IRQ_BASE_VECTOR               = 32;

	#else

	#endif
}


