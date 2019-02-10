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
			// SysEnterExit,

			SSE1,
			SSE2,
			SSE3,
			SupplementalSSE3,
			SSE41,
			SSE42,
			AES,
			AVX,
			RDRand,

			NX,
		};

		bool hasFeature(Feature f);

		bool didEnableNoExecute();
	}
}