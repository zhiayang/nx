// msr.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace cpu
{
	extern "C" uint64_t nx_x64_read_cr0();
	extern "C" uint64_t nx_x64_read_cr4();

	extern "C" void nx_x64_write_cr0(uint64_t x);
	extern "C" void nx_x64_write_cr4(uint64_t x);

	extern "C" uint64_t nx_x64_read_msr(uint32_t msr);
	extern "C" void nx_x64_write_msr(uint32_t msr, uint64_t val);

	uint64_t readCR0()          { return nx_x64_read_cr0();  }
	uint64_t readCR4()          { return nx_x64_read_cr4();  }
	uint64_t readMSR(uint32_t r){ return nx_x64_read_msr(r); }

	void writeCR0(uint64_t val) { nx_x64_write_cr0(val); }
	void writeCR4(uint64_t val) { nx_x64_write_cr4(val); }
	void writeMSR(uint32_t reg, uint64_t value)
	{
		nx_x64_write_msr(reg, value);
	}
}
}


















