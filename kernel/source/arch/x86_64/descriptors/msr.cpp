// msr.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace cpu
{
	uint64_t readCR0()          { return platform::read_cr0();  }
	uint64_t readCR4()          { return platform::read_cr4();  }
	uint64_t readMSR(uint32_t r){ return platform::read_msr(r); }

	void writeCR0(uint64_t val) { platform::write_cr0(val); }
	void writeCR4(uint64_t val) { platform::write_cr4(val); }
	void writeMSR(uint32_t reg, uint64_t value)
	{
		platform::write_msr(reg, value);
	}
}
}


















