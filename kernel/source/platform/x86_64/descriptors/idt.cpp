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

	constexpr int NumIDTEntries = 256;


	constexpr uint8_t FLAG_DPL_SHIFT    = 5;
	constexpr uint8_t FLAG_DPL_BITS     = 2;
	constexpr uint8_t FLAG_TYPE_BITS    = 4;
	constexpr uint8_t FLAG_TYPE_SHIFT   = 0;


	static IDTEntry idt[NumIDTEntries];
	static IDTPointer idtp;


	extern "C" void nx_x64_loadidt(uint64_t);


	void init()
	{
		idtp.base = (uintptr_t) &idt;
		idtp.limit = (sizeof(IDTEntry) * NumIDTEntries) - 1;

		memset(&idt, 0, NumIDTEntries * sizeof(IDTEntry));

		// load the IDT
		nx_x64_loadidt((uint64_t) &idtp);
	}





	void setEntry(uint8_t num, addr_t base, uint16_t codeSegment, bool ring3Interrupt, bool nestedInterrupts)
	{
		assert(num < 256);

		// The interrupt routine's base address
		idt[num].base_low   = (base & 0xFFFF);
		idt[num].base_mid   = (base >> 16) & 0xFFFF;
		idt[num].base_high  = (base >> 32) & 0xFFFFFFFF;

		// The segment or 'selector' that this IDT entry will use
		// is set here, along with any access flags

		idt[num].selector = codeSegment;
		if(num < IRQ_BASE_VECTOR)   idt[num].ist_offset = 0x0;		// ????? should be 1 or something.
		else                        idt[num].ist_offset = 0x0;

		idt[num].always0 = 0;

		uint8_t flags = 0x80;   // bit 7 is the present flag
		flags |= 0xE;           // bits 0:3 are the type. 0b1110 (ie 0xE) is interrupt gate -- no nested interrupts.

		if(ring3Interrupt)      flags |= 0x60;  // bits 5:6 are the ring (ie what ring will the cpu go into when interrupted)
		if(nestedInterrupts)    flags |= 0x1;   // setting bit 0 makes the type 0b1111 (ie 0xF), which is trap gate -- with nested interrupts

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




















