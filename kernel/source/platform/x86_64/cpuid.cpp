// cpuid.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "platform-specific.h"

extern "C" uint64_t __nx_x64_was_nx_bit_enabled;

namespace nx {
namespace cpu
{
	bool didEnableNoExecute()
	{
		if constexpr (getArchitecture() == Architecture::x64)
		{
			return __nx_x64_was_nx_bit_enabled;
		}
		else
		{
			return false;
		}
	}



	#define do_cpuid(num, a, b, c, d)   \
		asm volatile ("                 \
			cpuid" : "=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (num))


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

			case Feature::NX:
				do_cpuid(0x80000001, eax, ebx, ecx, edx);
				return edx & (1 << 20);

			case Feature::FSGSBase:
				do_cpuid(7, eax, ebx, ecx, edx);
				return ebx & (1 << 0);

			default:
				return false;
		}
	}
}
}

























