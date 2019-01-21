// apic.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "devices/acpi.h"
#include "devices/x64/apic.h"
#include "devices/x64/pic8259.h"

#include "cpu/cpuid.h"

namespace nx {
namespace device {
namespace apic
{
	static constexpr uint32_t IOAPIC_REG_ID             = 0;
	static constexpr uint32_t IOAPIC_REG_VERSION        = 1;
	static constexpr uint32_t IOAPIC_REG_ARBITRATION    = 2;

	static constexpr uint32_t IOAPIC_REG_IRQ_BASE       = 0x10;

	static array<IOAPIC> IoApics;
	static treemap<int, int> ISAIRQMapping;


	static void writeIOAPIC(IOAPIC* ioapic, uint32_t reg, uint32_t value)
	{
		// write to the selector first.
		*((volatile uint32_t*) ioapic->baseAddr) = reg & 0xFF;

		// write to the register.
		*((volatile uint32_t*) (ioapic->baseAddr + 0x10)) = value;
	}

	static uint32_t readIOAPIC(IOAPIC* ioapic, uint32_t reg)
	{
		// write to the selector first.
		*((volatile uint32_t*) ioapic->baseAddr) = reg & 0xFF;

		// return the value
		return *((volatile uint32_t*) (ioapic->baseAddr + 0x10));
	}









	// returns false if the system does not have an APIC!
	bool init()
	{
		// return false;

		// if we do not have ioapics, then we do not have lapics or whatever.
		// so, we return false -- falling back to the normal 8259 pic.
		if(IoApics.empty()) return false;


		// initialise all ioapics
		// reference: https://pdos.csail.mit.edu/6.828/2006/readings/ia32/ioapic.pdf
		// for the ioapic base address, the specs say FEC0 xy00h, where x is 0-F.
		// so, there are 16 possible pages we need to map (if the system has a fuckload of ioapics)
		// but, because there can be more than one ioapic in one page, and there's no guarantee that
		// we get them in any sorted order (eg. in increasing base address), we keep track of the physical
		// pages that we have already mapped, so we do not double map them (because we can't!)

		// note: "we can't" above refers to the fact that we need to allocate the virtual address space in the lower 4GB,
		// so if we've already allocated it then we can't allocate it again! (duh)

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

			// sanity check.
			int id = (readIOAPIC(ioa, IOAPIC_REG_ID) & 0xF000000) >> 24;
			if(id != ioa->id) abort("ioapic[%d]: mismatch in ioapic id! (found %d)", ioa->id, id);

			// get the number of interrupts this guy handles.
			{
				uint32_t reg = readIOAPIC(ioa, IOAPIC_REG_VERSION);
				ioa->maxRedirections = (reg & 0xFF0000) >> 16;

				log("ioapic", "[%d]: ver %x, %d intrs, gsi %d", ioa->id, reg & 0xFF, ioa->maxRedirections, ioa->gsiBase);
			}

			// set the spurious irq vector to 0xFF
		}

		return true;
	}


	void addISAIRQMapping(int isa, int irq)
	{
		if(auto it = ISAIRQMapping.find(isa); it != ISAIRQMapping.end())
			log("ioapic", "ISA IRQ %d already has a mapping to ioapic irq %d!", isa, irq);

		else
			ISAIRQMapping.insert(isa, irq);
	}

	int getISAIRQMapping(int isa)
	{
		if(auto it = ISAIRQMapping.find(isa); it != ISAIRQMapping.end())
			return it->value;

		else
			return -1;
	}


	static IOAPIC* getIoApicForIrq(int num)
	{
		// there should be a better way of doing this!!
		for(auto& ioapic : IoApics)
		{
			if(num >= ioapic.gsiBase && num < ioapic.gsiBase + ioapic.maxRedirections)
				return &ioapic;
		}

		warn("apic", "cannot handle irq %d because no ioapics handle it", num);
		return nullptr;
	}

	void setInterrupt(int irq, int vector, int apicId)
	{
		assert(vector < 0xFF);
		assert(apicId <= 0xF);

		// we add 32 to the vector, because we deal in irq-space -- not IDT space!
		// the ioapic will send the cpu an interrupt in 'idt-space', meaning the first
		// irq should send vector 32, not 0.
		vector += 32;


		auto ioa = getIoApicForIrq(irq);
		if(!ioa) return;

		// we need to subtract gsiBase from irq, because the ioapic can only have 24 interrupt pins
		// or something like that.

		irq -= ioa->gsiBase;

		// set all the flags first (but read that shit so we don't mess with the high/low level/edge stuff)
		uint32_t low = readIOAPIC(ioa, IOAPIC_REG_IRQ_BASE + (irq * 2));

		// save these so we can set them later.
		uint32_t polarity   = low & (1 << 13);
		uint32_t trigger    = low & (1 << 15);

		// reuse it.
		low = 0;

		// [0:7]    - interrupt vector
		// [8:10]   - delivery mode (0 for normal)
		// [11]     - destination mode (0 for physical)
		// [12]     - apic busy
		// [13]     - polarity
		// [14]     - ??
		// [15]     - trigger
		// [16]     - mask
		// [56:59]  - destination (4 bit apic id)

		low = (vector & 0xFF);
		low |= polarity;
		low |= trigger;

		// TODO: should we mask it by default? right now we don't.
		// low |= (1 << 16);

		uint32_t high = (apicId & 0xF) << 24;

		// write them back.
		writeIOAPIC(ioa, IOAPIC_REG_IRQ_BASE + (irq * 2) + 0, low);
		writeIOAPIC(ioa, IOAPIC_REG_IRQ_BASE + (irq * 2) + 1, high);

		// ok we should be done.
	}

	void maskIRQ(int num)
	{
		// because there's a lot of stuff in the register, we only mask/unmask the irq, and do nothing else
		// to the rest of the values.

		auto ioa = getIoApicForIrq(num);
		if(!ioa) return;

		// we only care about the low 32-bits to mask/unmask
		auto val = readIOAPIC(ioa, IOAPIC_REG_IRQ_BASE + (num * 2));
		val |= (1 << 16);

		writeIOAPIC(ioa, IOAPIC_REG_IRQ_BASE + (num * 2), val);
		log("apic", "irq %d was disabled", num);
	}

	void unmaskIRQ(int num)
	{
		auto ioa = getIoApicForIrq(num);
		if(!ioa) return;

		// we only care about the low 32-bits to mask/unmask
		auto val = readIOAPIC(ioa, IOAPIC_REG_IRQ_BASE + (num * 2));
		val &= ~(1 << 16);

		writeIOAPIC(ioa, IOAPIC_REG_IRQ_BASE + (num * 2), val);
		log("apic", "irq %d was enabled", num);
	}











	void preinit()
	{
		IoApics = array<IOAPIC>();
		ISAIRQMapping = treemap<int, int>();
	}

	void addIOAPIC(const IOAPIC& ioa)
	{
		IoApics.append(ioa);
	}

	size_t getNumIOAPICs()
	{
		return IoApics.size();
	}

}
}
}





















































