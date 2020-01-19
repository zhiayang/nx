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

		constexpr uint32_t MSR_STAR             = 0xC000'0081;
		constexpr uint32_t MSR_LSTAR            = 0xC000'0082;
		constexpr uint32_t MSR_CSTAR            = 0xC000'0083;
		constexpr uint32_t MSR_SF_MASK          = 0xC000'0084;

		constexpr uint32_t MSR_FS_BASE          = 0xC000'0100;
		constexpr uint32_t MSR_GS_BASE          = 0xC000'0101;
		constexpr uint32_t MSR_KERNEL_GS_BASE   = 0xC000'0102;

		constexpr uint32_t FLAG_CARRY           = (1UL << 0);
		constexpr uint32_t FLAG_PARITY          = (1UL << 2);
		constexpr uint32_t FLAG_ADJUST          = (1UL << 4);
		constexpr uint32_t FLAG_ZERO            = (1UL << 6);
		constexpr uint32_t FLAG_SIGN            = (1UL << 7);
		constexpr uint32_t FLAG_TRAP            = (1UL << 8);
		constexpr uint32_t FLAG_INTERRUPT       = (1UL << 9);
		constexpr uint32_t FLAG_DIRECTION       = (1UL << 10);
		constexpr uint32_t FLAG_OVERFLOW        = (1UL << 11);

		constexpr uint32_t CR0_PE               = (1UL << 0);   // protected mode enable
		constexpr uint32_t CR0_MP               = (1UL << 1);   // monitor coprocessor
		constexpr uint32_t CR0_EM               = (1UL << 2);   // emulate coprocessor
		constexpr uint32_t CR0_TS               = (1UL << 3);   // task switched
		constexpr uint32_t CR0_NE               = (1UL << 5);   // numeric exception
		constexpr uint32_t CR0_WP               = (1UL << 16);  // write-protect
		constexpr uint32_t CR0_NW               = (1UL << 29);  // no write-through cache
		constexpr uint32_t CR0_CD               = (1UL << 30);  // cache disable
		constexpr uint32_t CR0_PG               = (1UL << 31);  // paging enable

		constexpr uint32_t CR4_PSE              = (1UL << 4);   // page-size extension
		constexpr uint32_t CR4_PAE              = (1UL << 5);   // physical address extension
		constexpr uint32_t CR4_MCE              = (1UL << 6);   // machine-check exceptions
		constexpr uint32_t CR4_PGE              = (1UL << 7);   // page global enable
		constexpr uint32_t CR4_OSFXSR           = (1UL << 9);   // os-supported fxsave/fxrstor
		constexpr uint32_t CR4_OSXMMEXCPT       = (1UL << 10);  // os unmasked exception support
		constexpr uint32_t CR4_FSGSBASE         = (1UL << 16);  // (rd|wr)(fs|gs) base instructions
		constexpr uint32_t CR4_OSXSAVE          = (1UL << 18);  // xsave
		constexpr uint32_t CR4_SMEP             = (1UL << 20);  // supervisor mode execution prevention
		constexpr uint32_t CR4_SMAP             = (1UL << 21);  // supervisor mode access prevention

		uint64_t readMSR(uint32_t reg);
		void writeMSR(uint32_t reg, uint64_t value);

		uint64_t readCR0();
		void writeCR0(uint64_t val);

		uint64_t readCR4();
		void writeCR4(uint64_t val);

		uint64_t readFSBase();
		void writeFSBase(uint64_t base);

	}
}





