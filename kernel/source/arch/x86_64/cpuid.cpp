// cpuid.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "platform-specific.h"

extern "C" uint64_t __nx_x64_was_nx_bit_enabled;
extern "C" uint64_t __nx_x64_have_cmpxchg16b;

namespace nx {
namespace cpu
{
	bool didEnableNoExecute()
	{
		return __nx_x64_was_nx_bit_enabled;
	}

	bool supportsDoubleWordCompareExchange()
	{
		return __nx_x64_have_cmpxchg16b;
	}



	static uint32_t MaxSupportedLeaf = 0;
	static uint32_t MaxSupportedExtLeaf = 0;

	void initCPUID()
	{
		do_cpuid(0, &MaxSupportedLeaf, 0, 0, 0);
		do_cpuid(0x80000000, &MaxSupportedExtLeaf, 0, 0, 0);

		log("cpuid", "max supported leaf = {} (extended = {x})", MaxSupportedLeaf, MaxSupportedExtLeaf);
	}


	bool do_cpuid(uint32_t leaf, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d)
	{
		// 0 < leaf < max, and 0x80000000 < leaf < maxExt
		if((leaf < 0x80000000 && leaf > MaxSupportedLeaf) || (leaf > 0x80000000 && leaf > MaxSupportedExtLeaf))
			return false;

		register uint32_t eax asm("eax") = leaf;
		register uint32_t ebx asm("ebx") = 0;
		register uint32_t ecx asm("ecx") = 0;
		register uint32_t edx asm("edx") = 0;

		asm volatile ("cpuid"   : "=r"(eax), "=r"(ebx), "=r"(ecx), "=r"(edx)
								: "r"(eax));

		if(a) *a = eax;
		if(b) *b = ebx;
		if(c) *c = ecx;
		if(d) *d = edx;

		return true;
	}

	bool do_cpuid(uint32_t leaf, uint32_t subleaf, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d)
	{
		// 0 < leaf < max, and 0x80000000 < leaf < maxExt
		if((leaf < 0x80000000 && leaf > MaxSupportedLeaf) || (leaf > 0x80000000 && leaf > MaxSupportedExtLeaf))
			return false;

		register uint32_t eax asm("eax") = leaf;
		register uint32_t ebx asm("ebx") = 0;
		register uint32_t ecx asm("ecx") = subleaf;
		register uint32_t edx asm("edx") = 0;

		asm volatile ("cpuid"   : "=r"(eax), "=r"(ebx), "=r"(ecx), "=r"(edx)
								: "r"(eax), "r"(ecx));

		if(a) *a = eax;
		if(b) *b = ebx;
		if(c) *c = ecx;
		if(d) *d = edx;

		return true;
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
				return do_cpuid(1, &eax, &ebx, &ecx, &edx) && (edx & (1 << 4));

			case Feature::LocalAPIC:
				return do_cpuid(1, &eax, &ebx, &ecx, &edx) && (edx & (1 << 9));

			case Feature::PageGlobalEnable:
				return do_cpuid(1, &eax, &ebx, &ecx, &edx) && (edx & (1 << 13));

			case Feature::SSE1:
				return do_cpuid(1, &eax, &ebx, &ecx, &edx) && (edx & (1 << 25));

			case Feature::SSE2:
				return do_cpuid(1, &eax, &ebx, &ecx, &edx) && (edx & (1 << 26));

			case Feature::SimultaneousMultiThreading:
				return do_cpuid(1, &eax, &ebx, &ecx, &edx) && (edx & (1 << 28));

			case Feature::SSE3:
				return do_cpuid(1, &eax, &ebx, &ecx, &edx) && (ecx & (1 << 0));

			case Feature::SupplementalSSE3:
				return do_cpuid(1, &eax, &ebx, &ecx, &edx) && (ecx & (1 << 9));

			case Feature::SSE41:
				return do_cpuid(1, &eax, &ebx, &ecx, &edx) && (ecx & (1 << 19));

			case Feature::SSE42:
				return do_cpuid(1, &eax, &ebx, &ecx, &edx) && (ecx & (1 << 20));

			case Feature::x2LocalAPIC:
				return do_cpuid(1, &eax, &ebx, &ecx, &edx) && (ecx & (1 << 21));

			case Feature::AES:
				return do_cpuid(1, &eax, &ebx, &ecx, &edx) && (ecx & (1 << 25));

			case Feature::AVX:
				return do_cpuid(1, &eax, &ebx, &ecx, &edx) && (ecx & (1 << 28));

			case Feature::RDRand:
				return do_cpuid(1, &eax, &ebx, &ecx, &edx) && (ecx & (1 << 30));

			case Feature::NX:
				return do_cpuid(0x80000001, &eax, &ebx, &ecx, &edx) && (edx & (1 << 20));

			case Feature::FSGSBase:
				return do_cpuid(7, &eax, &ebx, &ecx, &edx) && (ebx & (1 << 0));

			case Feature::FXSaveRestore:
				return do_cpuid(1, &eax, &ebx, &ecx, &edx) && (edx & (1 << 24));

			case Feature::XSave:
				return do_cpuid(1, &eax, &ebx, &ecx, &edx) && (ecx & (1 << 26));

			case Feature::OSXSave:
				return do_cpuid(1, &eax, &ebx, &ecx, &edx) && (ecx & (1 << 27));

			default:
				return false;
		}
	}
}
}

























