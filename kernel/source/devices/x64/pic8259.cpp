// pic8259.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace device {
namespace pic8259
{
	static constexpr uint8_t PIC1_CMD   = 0x20;
	static constexpr uint8_t PIC1_DATA  = 0x21;
	static constexpr uint8_t PIC2_CMD   = 0xA0;
	static constexpr uint8_t PIC2_DATA  = 0xA1;

	void init()
	{
		// Remap the IRQs from 0 - 7 -> 8 - 15 to 32-47
		port::write1b(PIC1_CMD, 0x11);
		port::write1b(PIC2_CMD, 0x11);
		port::write1b(PIC1_DATA, 0x20);
		port::write1b(PIC2_DATA, 0x28);
		port::write1b(PIC1_DATA, 0x04);
		port::write1b(PIC2_DATA, 0x02);
		port::write1b(PIC1_DATA, 0x01);
		port::write1b(PIC2_DATA, 0x01);
		port::write1b(PIC1_DATA, 0x0);
		port::write1b(PIC2_DATA, 0x0);
	}

	void disable()
	{
		// when no command is issued, writing to the data port sets the interrupt
		// masks. writing 0xFF masks all interrupts, effectively disabling the PIC.
		port::write1b(PIC1_DATA, 0xFF);
		port::write1b(PIC2_DATA, 0xFF);
	}
}
}
}



















