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


	void do_cpuid(uint32_t leaf, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d)
	{
		register uint32_t eax asm("eax") = leaf;
		register uint32_t ebx asm("ebx") = 0;
		register uint32_t ecx asm("ecx") = 0;
		register uint32_t edx asm("edx") = 0;

		asm volatile ("cpuid"   : "=r"(eax), "=r"(ebx), "=r"(ecx), "=r"(edx)
								: "r"(eax));

		*a = eax;
		*b = ebx;
		*c = ecx;
		*d = edx;
	}



	bool hasFeature(Feature f)
	{
		uint32_t eax = 0;
		uint32_t ebx = 0;
		uint32_t ecx = 0;
		uint32_t edx = 0;

		switch(f)
		{
			case Feature::TimeStampCounter:
				do_cpuid(1, &eax, &ebx, &ecx, &edx);
				return edx & (1 << 4);

			case Feature::LocalAPIC:
				do_cpuid(1, &eax, &ebx, &ecx, &edx);
				return edx & (1 << 9);

			case Feature::PageGlobalEnable:
				do_cpuid(1, &eax, &ebx, &ecx, &edx);
				return edx & (1 << 13);

			case Feature::SSE1:
				do_cpuid(1, &eax, &ebx, &ecx, &edx);
				return edx & (1 << 25);

			case Feature::SSE2:
				do_cpuid(1, &eax, &ebx, &ecx, &edx);
				return edx & (1 << 26);

			case Feature::SimultaneousMultiThreading:
				do_cpuid(1, &eax, &ebx, &ecx, &edx);
				return edx & (1 << 28);

			case Feature::SSE3:
				do_cpuid(1, &eax, &ebx, &ecx, &edx);
				return ecx & (1 << 0);

			case Feature::SupplementalSSE3:
				do_cpuid(1, &eax, &ebx, &ecx, &edx);
				return ecx & (1 << 9);

			case Feature::SSE41:
				do_cpuid(1, &eax, &ebx, &ecx, &edx);
				return ecx & (1 << 19);

			case Feature::SSE42:
				do_cpuid(1, &eax, &ebx, &ecx, &edx);
				return ecx & (1 << 20);

			case Feature::x2LocalAPIC:
				do_cpuid(1, &eax, &ebx, &ecx, &edx);
				return ecx & (1 << 21);

			case Feature::AES:
				do_cpuid(1, &eax, &ebx, &ecx, &edx);
				return ecx & (1 << 25);

			case Feature::AVX:
				do_cpuid(1, &eax, &ebx, &ecx, &edx);
				return ecx & (1 << 28);

			case Feature::RDRand:
				do_cpuid(1, &eax, &ebx, &ecx, &edx);
				return ecx & (1 << 30);

			case Feature::NX:
				do_cpuid(0x80000001, &eax, &ebx, &ecx, &edx);
				return edx & (1 << 20);

			case Feature::FSGSBase:
				do_cpuid(7, &eax, &ebx, &ecx, &edx);
				return ebx & (1 << 0);

			default:
				return false;
		}
	}
}
}

























