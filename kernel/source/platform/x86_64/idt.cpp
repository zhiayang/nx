// idt.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace cpu {
namespace idt
{
	struct IDTEntry
	{
		uint16_t    base_low;
		uint16_t    selector;
		uint8_t     ist_offset;
		uint8_t     flags;
		uint16_t    base_mid;
		uint32_t    base_high;
		uint32_t    always0;
	} __attribute__((packed));

	struct IDTPointer
	{
		uint16_t limit;
		uint64_t base;
	} __attribute__((packed));

	static constexpr int NumIDTEntries = 256;

	static IDTEntry idt[NumIDTEntries];
	static IDTPointer idtp;


	extern "C" void nx_x64_loadidt(uint64_t);


	void init()
	{
		idtp.limit = (sizeof(IDTEntry) * NumIDTEntries) - 1;
		idtp.base = (uintptr_t) &idt;

		memset(&idt, 0, NumIDTEntries * sizeof(IDTEntry));

		// load the IDT
		nx_x64_loadidt((uint64_t) &idtp);
	}





	void setEntry(uint8_t num, addr_t base, uint16_t codeSegment, uint8_t flags)
	{
		assert(num < 256);

		// The interrupt routine's base address
		idt[num].base_low   = (base & 0xFFFF);
		idt[num].base_mid   = (base >> 16) & 0xFFFF;
		idt[num].base_high  = (base >> 32) & 0xFFFFFFFF;

		// The segment or 'selector' that this IDT entry will use
		// is set here, along with any access flags

		idt[num].selector = codeSegment;
		if(num < 32)    idt[num].ist_offset = 0x0;		// ????? should be 1 or something.
		else            idt[num].ist_offset = 0x0;

		idt[num].always0 = 0;
		idt[num].flags = flags;
	}

	void enableGate(uint8_t num)
	{
		assert(num < 256);
		idt[num].flags |= 0x80;
	}

	void disableGate(uint8_t num)
	{
		assert(num < 256);
		idt[num].flags &= ~0x80;
	}
}
}
}




















