// x64/cpuid.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "defs.h"

namespace nx
{
	namespace cpu
	{
		enum class Feature
		{
			TimeStampCounter,
			LocalAPIC,
			SimultaneousMultiThreading,     // aka intel hyperthreading / amd SMT
			x2LocalAPIC,                    // version 2 of apic (since ~2008)
			PageGlobalEnable,

			FSGSBase,

			SSE1,
			SSE2,
			SSE3,
			SupplementalSSE3,
			SSE41,
			SSE42,
			AES,
			AVX,
			RDRand,

			FXSaveRestore,
			XSave,
			OSXSave,

			NX,
		};

		void initCPUID();
		bool do_cpuid(uint32_t leaf, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d);
		bool do_cpuid(uint32_t leaf, uint32_t subleaf, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d);

		bool hasFeature(Feature f);

		bool didEnableNoExecute();
	}
}
