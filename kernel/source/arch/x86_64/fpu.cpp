// fpu.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace cpu {
namespace fpu
{
	constexpr uint32_t XSAVE_CPUID_LEAF             = 0x0D;

	// bit masks for xsave
	constexpr uint64_t XSAVE_STATE_BIT_X87                  = (1 << 0);
	constexpr uint64_t XSAVE_STATE_BIT_SSE                  = (1 << 1);
	constexpr uint64_t XSAVE_STATE_BIT_AVX                  = (1 << 2);
	constexpr uint64_t XSAVE_STATE_BIT_MPX_BNDREG           = (1 << 3);
	constexpr uint64_t XSAVE_STATE_BIT_MPX_BNDCSR           = (1 << 4);
	constexpr uint64_t XSAVE_STATE_BIT_AVX512_OPMASK        = (1 << 5);
	constexpr uint64_t XSAVE_STATE_BIT_AVX512_LOWERZMM_HIGH = (1 << 6);
	constexpr uint64_t XSAVE_STATE_BIT_AVX512_HIGHERZMM     = (1 << 7);
	constexpr uint64_t XSAVE_STATE_BIT_PT                   = (1 << 8);
	constexpr uint64_t XSAVE_STATE_BIT_PKRU                 = (1 << 9);

	constexpr size_t XSAVE_EXTENDED_AREA_OFFSET     = 576;
	constexpr size_t XSAVE_MAX_COMPONENTS           = 63;
	constexpr size_t XSAVE_FIRST_EXTENDED_COMPONENT = 2;

	static size_t XSaveStateMaxSize     = 0;

	static uint64_t XSSComponentBitmap  = 0;
	static uint64_t XCR0ComponentBitmap = 0;

	static bool IsFXSAVE_Supported      = false;
	static bool IsXSAVE_Supported       = false;
	static bool IsXSAVES_Supported      = false;
	static bool IsXGETBV1_Supported     = false;
	static bool IsXSAVEOPT_Supported    = false;

	static struct {
		size_t size;
		bool aligned64;
	} XSaveStateComponents[XSAVE_MAX_COMPONENTS];

	static uint8_t InitialXSaveState[512 + 64] = { };


	static inline uint64_t xgetbv(uint32_t reg)
	{
		uint32_t hi = 0, lo = 0;
		asm volatile ("xgetbv" : "=d"(hi), "=a"(lo) : "c"(reg) : "memory");
		return ((uint64_t) hi << 32) + lo;
	}

	static inline void xsetbv(uint32_t reg, uint64_t val)
	{
		asm volatile ("xsetbv" : : "c"(reg), "d"((uint32_t) (val >> 32)), "a"((uint32_t) val) : "memory");
	}

	static inline void fxsave(void* buffer)
	{
		asm volatile ("fxsave64 %0" : "=m"(*(uint8_t*) buffer) :: "memory");
	}

	static inline void fxrstor(void* buffer)
	{
		asm volatile ("fxrstor64 %0" :: "m"(*(uint8_t*) buffer) : "memory");
	}

	static inline void xrstor(void* buffer, uint64_t feature_mask)
	{
		asm volatile ("xrstor %0" :: "m"(*(uint8_t*) buffer), "d"((uint32_t) (feature_mask >> 32)), "a"((uint32_t) feature_mask) : "memory");
	}

	static inline void xrstors(void* buffer, uint64_t feature_mask)
	{
		asm volatile ("xrstors %0" :: "m"(*(uint8_t*) buffer), "d"((uint32_t) (feature_mask >> 32)), "a"((uint32_t) feature_mask) : "memory");
	}

	static inline void xsave(void* buffer, uint64_t feature_mask)
	{
		asm volatile ("xsave %0" : "+m"(*(uint8_t*) buffer) : "d"((uint32_t) (feature_mask >> 32)), "a"((uint32_t) feature_mask) : "memory");
	}

	static inline void xsaveopt(void* buffer, uint64_t feature_mask)
	{
		asm volatile ("xsaveopt %0" : "+m"(*(uint8_t*) buffer) : "d"((uint32_t) (feature_mask >> 32)), "a"((uint32_t) feature_mask) : "memory");
	}

	static inline void xsaves(void* buffer, uint64_t feature_mask)
	{
		asm volatile ("xsaves %0" : "+m"(*(uint8_t*) buffer) : "d"((uint32_t) (feature_mask >> 32)), "a"((uint32_t) feature_mask) : "memory");
	}






	static void determineXSaveSize()
	{
		uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;

		if(!do_cpuid(XSAVE_CPUID_LEAF, &eax, &ebx, &ecx, &edx))
		{
			error("fpu", "xsave is supported, but cpuid leaf was not found");
			return;
		}

		XCR0ComponentBitmap = (((uint64_t) edx) << 32) | (eax);
		XSaveStateMaxSize = XSAVE_EXTENDED_AREA_OFFSET;

		do_cpuid(XSAVE_CPUID_LEAF, 1, &eax, &ebx, &ecx, &edx);
		IsXSAVES_Supported      = (eax & (1 << 3));
		IsXGETBV1_Supported     = (eax & (1 << 2));
		IsXSAVEOPT_Supported    = (eax & (1 << 0));

		XSSComponentBitmap      = (((uint64_t) edx) << 32) | (ecx);

		// xcr0 component bitmap lists the component indices that the cpu supports
		// all cpus with xsave must support saving the legacy sse state, which are
		// components 0 and 1
		assert((XCR0ComponentBitmap & 0x3) == 0x3);


		// get information about each component.
		for(size_t i = XSAVE_FIRST_EXTENDED_COMPONENT; i < XSAVE_MAX_COMPONENTS; i++)
		{
			// if it's not supported by the cpu, skip it.
			if(!(XCR0ComponentBitmap & (1ULL << i)) && !(XSSComponentBitmap & (1ULL << i)))
				continue;

			do_cpuid(XSAVE_CPUID_LEAF, i, &eax, &ebx, &ecx, &edx);

			XSaveStateComponents[i].size        = eax;
			XSaveStateComponents[i].aligned64   = (ecx & 0x2);


			// if the alignment flag is set, this component must be 64-byte aligned
			// so we need to round up the current size (ie. offset)
			if(XSaveStateComponents[i].aligned64)
				XSaveStateMaxSize = ((XSaveStateMaxSize + 63) / 64) * 64;

			// note that we are not rounding up *our* size, because the next component might
			// not need to be 64-byte aligned.
			XSaveStateMaxSize += XSaveStateComponents[i].size;
		}

		log("fpu", "xsave size: %zu", XSaveStateMaxSize);
	}

	static void enableFPU()
	{
		// enable the fpu
		{
			auto cr0 = readCR0();
			cr0 &= ~CR0_EM;
			cr0 |=  CR0_NE;
			cr0 |=  CR0_MP;
			writeCR0(cr0);

			asm volatile ("finit" ::: "memory");
		}

		// enable sse
		{
			writeCR4(readCR4() | CR4_OSFXSR | CR4_OSXMMEXCPT);

			uint32_t mxcsr = (0x3F << 7);
			asm volatile ("ldmxcsr %0" :: "m"(mxcsr));
		}

		if(IsXSAVE_Supported)
		{
			writeCR4(readCR4() | CR4_OSXSAVE);
			xsetbv(0, XSAVE_STATE_BIT_X87 | XSAVE_STATE_BIT_SSE);
		}
	}

	static void setupInitialState()
	{
		auto area = &((xsave_state*) &InitialXSaveState[0])->fxsaveRegion;

		// set the x87 state:
		area->fcw = 0x037F;
		area->fsw = 0;
		area->ftw = 0;
		area->fop = 0;
		area->fip = 0;
		area->fdp = 0;

		// set the st registers to 0
		memset(&area->mmxRegs[0], 0, 128);

		// set the xmm registers to 0 also
		memset(&area->xmmRegs[0], 0, 256);

		// idk what mxcsr does
		area->mxcsr = 0x3F << 7;
	}


	void init()
	{
		IsFXSAVE_Supported = cpu::hasFeature(Feature::FXSaveRestore);
		IsXSAVE_Supported = cpu::hasFeature(Feature::XSave);


		if(!IsFXSAVE_Supported)
		{
			error("fpu", "cpu does not support fxsave/fxrstor, aborting fpu initialisation");
			return;
		}

		enableFPU();


		if(IsXSAVE_Supported)
			determineXSaveSize();

		setupInitialState();

		if(IsXSAVE_Supported)
		{
			auto area = (xsave_state*) &InitialXSaveState[0];
			area->xstateBV |= XSAVE_STATE_BIT_SSE;

			if(IsXSAVES_Supported)
			{
				area->xcompBV |= (1ULL << 63);      // enable the compacted format
				area->xcompBV |= area->xstateBV;
			}
		}
	}

	size_t getFPUStateSize()
	{
		return XSaveStateMaxSize;
	}

	void initState(addr_t buffer)
	{
		memmove((void*) buffer, (void*) &InitialXSaveState[0], sizeof(InitialXSaveState));
	}









	void save(addr_t base)
	{
		assert(base);

		if(IsXSAVES_Supported)              xsaves((void*) base, ~0ULL);
		else if(IsXSAVEOPT_Supported)       xsaveopt((void*) base, ~0ULL);
		else if(IsXSAVE_Supported)          xsave((void*) base, ~0ULL);
		else if(IsFXSAVE_Supported)         fxsave((void*) base);
	}

	void restore(addr_t base)
	{
		assert(base);

		if(IsXSAVES_Supported)              xrstors((void*) base, ~0ULL);
		else if(IsXSAVE_Supported)          xrstor((void*) base, ~0ULL);
		else if(IsFXSAVE_Supported)         fxrstor((void*) base);
	}









}
}
}
