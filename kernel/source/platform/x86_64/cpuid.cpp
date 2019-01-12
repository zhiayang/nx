// cpuid.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "defs.h"
#include "cpu/cpuid.h"

namespace nx {
namespace cpu
{
	#define do_cpuid(num, a, b, c, d) \
		asm volatile ("xchgl %%ebx, %1; cpuid; xchgl %%ebx, %1" \
        	: "=a" (a), "=r" (b), "=c" (c), "=d" (d)  \
            : "0" (num))

	bool hasFeature(Feature f)
	{
		int eax = 0, ebx = 0, ecx = 0, edx = 0;
		switch(f)
		{
			case Feature::TimeStampCounter:
				do_cpuid(1, eax, ebx, ecx, edx);
				return edx & (1 << 4);

			case Feature::LocalAPIC:
				do_cpuid(1, eax, ebx, ecx, edx);
				return edx & (1 << 9);

			case Feature::PageGlobalEnable:
				do_cpuid(1, eax, ebx, ecx, edx);
				return edx & (1 << 13);

			case Feature::SSE1:
				do_cpuid(1, eax, ebx, ecx, edx);
				return edx & (1 << 25);

			case Feature::SSE2:
				do_cpuid(1, eax, ebx, ecx, edx);
				return edx & (1 << 26);

			case Feature::SimultaneousMultiThreading:
				do_cpuid(1, eax, ebx, ecx, edx);
				return edx & (1 << 28);

			case Feature::SSE3:
				do_cpuid(1, eax, ebx, ecx, edx);
				return ecx & (1 << 0);

			case Feature::SupplementalSSE3:
				do_cpuid(1, eax, ebx, ecx, edx);
				return ecx & (1 << 9);

			case Feature::SSE41:
				do_cpuid(1, eax, ebx, ecx, edx);
				return ecx & (1 << 19);

			case Feature::SSE42:
				do_cpuid(1, eax, ebx, ecx, edx);
				return ecx & (1 << 20);

			case Feature::x2LocalAPIC:
				do_cpuid(1, eax, ebx, ecx, edx);
				return ecx & (1 << 21);

			case Feature::AES:
				do_cpuid(1, eax, ebx, ecx, edx);
				return ecx & (1 << 25);

			case Feature::AVX:
				do_cpuid(1, eax, ebx, ecx, edx);
				return ecx & (1 << 28);

			case Feature::RDRand:
				do_cpuid(1, eax, ebx, ecx, edx);
				return ecx & (1 << 30);

			default:
				return false;
		}
	}


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

		asm volatile ("wrmsr" : : "a"(low), "d"(high), "c"(reg));
	}
}
}

























