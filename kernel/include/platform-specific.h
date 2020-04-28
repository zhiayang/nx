// platform-specific.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>

namespace nx
{
#ifdef __ARCH_x64__

	constexpr int IRQ_BASE_VECTOR = 32;

	constexpr bool IS_CANONICAL(uintptr_t x)
	{
		return (x >= 0xFFFF'8000'0000'0000) || x <= (0x7FFF'FFFF'FFFF);
	}

	namespace platform
	{
		uint64_t get_flags_register();

		bool is_interrupted();
		void enter_interrupt_context();
		void leave_interrupt_context();

		uint64_t read_cr0();
		uint64_t read_cr4();

		void write_cr0(uint64_t x);
		void write_cr4(uint64_t x);

		uint64_t read_msr(uint32_t msr);
		void write_msr(uint32_t msr, uint64_t val);

		void load_gdt(uint64_t gdtp);
		void load_idt(uint64_t idtp);
		void load_tss(uint16_t sel);

		uint64_t get_gsbase();

		uint64_t rdfsbase();
		void wrfsbase(uint64_t value);
	}

#else

#endif
}


