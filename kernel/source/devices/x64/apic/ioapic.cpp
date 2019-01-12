// apic.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "devices/acpi.h"
#include "devices/x64/apic.h"

#include "cpu/cpuid.h"

namespace nx {
namespace apic
{
	static constexpr uint32_t IOAPIC_REG_ID             = 0;
	static constexpr uint32_t IOAPIC_REG_VERSION        = 1;
	static constexpr uint32_t IOAPIC_REG_ARBITRATION    = 2;

	static array<IOAPIC> IoApics;

	void init()
	{
		// initialise all ioapics
		// reference: https://pdos.csail.mit.edu/6.828/2006/readings/ia32/ioapic.pdf
		// for the ioapic base address, the specs say FEC0 xy00h, where x is 0-F.
		// so, there are 16 possible pages we need to map (if the system has a fuckload of ioapics)
		// but, because there can be more than one ioapic in one page, and there's no guarantee that
		// we get them in any sorted order (eg. in increasing base address), we keep track of the physical
		// pages that we have already mapped, so we do not double map them (because we can't!)

		array<addr_t> mappedBases;
		for(auto& _ioa : IoApics)
		{
			auto ioa = &_ioa;

			if(mappedBases.find(ioa->baseAddr & vmm::PAGE_ALIGN) == mappedBases.end())
			{
				auto alignedBase = ioa->baseAddr & vmm::PAGE_ALIGN;
				if(vmm::allocateSpecific(alignedBase, 1) == 0)
					abort("ioapic: failed to map base address %p", ioa->baseAddr);

				vmm::mapAddress(alignedBase, alignedBase, 1, vmm::PAGE_PRESENT | vmm::PAGE_WRITE);
				mappedBases.append(alignedBase);
			}

			int id = (readIOAPIC(ioa, IOAPIC_REG_ID) & 0xF000000) >> 24;
			if(id != ioa->id) abort("ioapic[%d]: mismatch in ioapic id! (found %d)", ioa->id, id);


			{
				uint32_t reg = readIOAPIC(ioa, IOAPIC_REG_VERSION);
				ioa->maxRedirections = (reg & 0xFF0000) >> 16;

				println("ioapic[%d]: ver %x, %d intrs", ioa->id, reg & 0xFF, ioa->maxRedirections);
			}
		}
	}












	void preinit()
	{
		IoApics = array<IOAPIC>();
	}

	void addIOAPIC(const IOAPIC& ioa)
	{
		IoApics.append(ioa);
	}

	size_t getNumIOAPICs()
	{
		return IoApics.size();
	}

	void writeIOAPIC(IOAPIC* ioapic, uint32_t reg, uint32_t value)
	{
		// write to the selector first.
		*((volatile uint32_t*) ioapic->baseAddr) = reg & 0xFF;

		// write to the register.
		*((volatile uint32_t*) (ioapic->baseAddr + 0x10)) = value;
	}

	uint32_t readIOAPIC(IOAPIC* ioapic, uint32_t reg)
	{
		// write to the selector first.
		*((volatile uint32_t*) ioapic->baseAddr) = reg & 0xFF;

		// return the value
		return *((volatile uint32_t*) (ioapic->baseAddr + 0x10));
	}
}


























namespace interrupts
{
	void init()
	{
		// TODO: probably do different stuff on different platforms
		apic::init();
	}

	void enable()
	{
	}

	void disable()
	{
	}
}
}






























