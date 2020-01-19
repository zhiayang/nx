// msr.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace cpu
{
	uint64_t readMSR(uint32_t reg)
	{
		uint32_t low = 0;
		uint32_t high = 0;

		asm volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(reg));

		return ((uint64_t) high << 32) | low;
	}

	void writeMSR(uint32_t reg, uint64_t value)
	{
		uint32_t low = value & 0xFFFFFFFF;
		uint32_t high = (value & 0xFFFFFFFF'00000000) >> 32;

		asm volatile ("wrmsr" :: "a"(low), "d"(high), "c"(reg));
	}

	uint64_t readCR0()
	{
		uint64_t ret = 0;
		asm volatile ("mov %%cr0, %0" : "=a"(ret));
		return ret;
	}

	void writeCR0(uint64_t val)
	{
		asm volatile ("mov %0, %%cr0" :: "a"(val));
	}

	uint64_t readCR4()
	{
		uint64_t ret = 0;
		asm volatile ("mov %%cr4, %0" : "=a"(ret));
		return ret;
	}

	void writeCR4(uint64_t val)
	{
		asm volatile ("mov %0, %%cr4" :: "a"(val));
	}
}
}


















