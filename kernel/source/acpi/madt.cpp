// madt.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "cpu/cpuid.h"

namespace nx {
namespace acpi
{
	static constexpr uint8_t MADT_ENTRY_TYPE_LAPIC      = 0;
	static constexpr uint8_t MADT_ENTRY_TYPE_IOAPIC     = 1;
	static constexpr uint8_t MADT_ENTRY_TYPE_INT_SRC    = 2;
	static constexpr uint8_t MADT_ENTRY_TYPE_NMI        = 4;
	static constexpr uint8_t MADT_ENTRY_TYPE_LAPIC_ADDR = 5;


	void readMADT(MADTable* madt)
	{
		if(!cpu::hasFeature(cpu::Feature::LocalAPIC))
			abort("cpu does not have a local apic, but madt was present??");

		if(!checkTable((header_t*) madt))
		{
			println("acpi/madt: invalid checksum, skipping");
			return;
		}

		apic::preinit();

		size_t len = madt->header.length;
		size_t ofs = sizeof(MADTable);

		addr_t lApicAddr = madt->localApicAddr;
		{
			// search first for an override.
			auto x = ofs;
			while(x < len)
			{
				auto rec = (madt_record_header_t*) ((addr_t) madt + x);
				if(rec->type == MADT_ENTRY_TYPE_LAPIC_ADDR)
				{
				    lApicAddr = ((MADT_LAPICOverride*) rec)->lapicAddress;
					break;
				}

				x += rec->length;
			}
		}

		println("acpi/madt: lapic at %p", lApicAddr);

		bool foundBsp = false;
		while(ofs < len)
		{
			// ACPI spec says we should initialise processors in the order
			// that they appear, and that the first processor is the bootstrap
			// processor!

			auto rec = (madt_record_header_t*) ((addr_t) madt + ofs);
			if(rec->type == MADT_ENTRY_TYPE_LAPIC)
			{
				auto lapic = (MADT_LAPIC*) rec;
				if(lapic->flags & 0x1)  // bit 0 is set if the processor is enabled.
				{
					scheduler::registerProcessor(!foundBsp, lapic->processorId, lapic->apicId, lApicAddr);
					foundBsp = true;
				}
			}
			else if(rec->type == MADT_ENTRY_TYPE_IOAPIC)
			{
				auto ioapic = (MADT_IOAPIC*) rec;

				apic::IOAPIC ioa;
				ioa.id = ioapic->id;
				ioa.baseAddr = (addr_t) ioapic->ioApicAddress;
				ioa.gsiBase = (addr_t) ioapic->globalSysInterruptBase;

				println("acpi/madt: ioapic[%d] at %p", ioa.id, ioa.baseAddr);

				apic::addIOAPIC(ioa);
			}

			ofs += rec->length;
		}


		println("acpi/madt: found %s, %s", util::plural("processor", scheduler::getNumProcessors()).cstr(),
			util::plural("ioapic", apic::getNumIOAPICs()).cstr());

		{
			// confirm our suspicions. we can also get the local apic base address from an MSR (0x1B).
			// it is page-aligned, so there are some flags in the lower 12 bits:
			// bit 8 is set if this is the BSP (it should be, because we have not started any APs yet)
			// bit 11 is set if the LAPIC is already enabled.

			addr_t base = cpu::readMSR(0x1B);
			if((base & vmm::PAGE_ALIGN) != lApicAddr)
			{
				abort("acpi/madt: invalid configuration, lapic base address in MSR (%p) does not match madt (%p)",
					base & vmm::PAGE_ALIGN, lApicAddr);
			}
			else if(!(base & 0x100))
			{
				abort("acpi/madt: lapic msr says we are not the bootstrap processor??");
			}

			base &= vmm::PAGE_ALIGN;
			base |= 0x800;
			base |= 0x100;

			cpu::writeMSR(0x1B, base);

			// note: this 'enabling' doesn't start accepting interrupts; it just means 'not disabled'.
			// we actually enable it later.
		}
	}
}
}




















