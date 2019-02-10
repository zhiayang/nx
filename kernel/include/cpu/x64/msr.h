// x64/msr.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include "defs.h"

namespace nx
{
	namespace cpu
	{
		constexpr uint32_t MSR_APIC_BASE        = 0x1B;

		constexpr uint32_t MSR_EFER             = 0xC000'0080;

		constexpr uint32_t MSR_LSTAR            = 0xC000'0082;
		constexpr uint32_t MSR_CSTAR            = 0xC000'0083;
		constexpr uint32_t MSR_SF_MASK          = 0xC000'0084;

		constexpr uint32_t MSR_FS_BASE          = 0xC000'0100;
		constexpr uint32_t MSR_GS_BASE          = 0xC000'0101;
		constexpr uint32_t MSR_KERNEL_GS_BASE   = 0xC000'0102;

		uint64_t readMSR(uint32_t reg);
		void writeMSR(uint32_t reg, uint64_t value);
	}
}