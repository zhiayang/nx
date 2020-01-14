// madt.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "devices/pc/apic.h"

namespace nx {
namespace acpi
{
	constexpr uint8_t MADT_ENTRY_TYPE_LAPIC      = 0;
	constexpr uint8_t MADT_ENTRY_TYPE_IOAPIC     = 1;
	constexpr uint8_t MADT_ENTRY_TYPE_INT_SRC    = 2;
	constexpr uint8_t MADT_ENTRY_TYPE_NMI        = 4;
	constexpr uint8_t MADT_ENTRY_TYPE_LAPIC_ADDR = 5;

	void readMADT(MADTable* madt)
	{
		if(!cpu::hasFeature(cpu::Feature::LocalAPIC))
		{
			error("acpi", "cpu does not have a local apic, ignoring MADT");
			return;
		}

		if(!checkTable((header_t*) madt))
		{
			error("acpi", "madt has invalid checksum, skipping");
			return;
		}

		device::ioapic::preinit();

		auto iterateTables = [](MADTable* madt, uint8_t type, auto callback) {
			size_t len = madt->header.length;
			addr_t base = ((addr_t) madt);

			size_t ofs = sizeof(MADTable);
			while(ofs < len)
			{
				auto rec = (madt_record_header_t*) (base + ofs);
				if(rec->type == type)
					callback(rec);

				ofs += rec->length;
			}
		};


		addr_t lApicAddr = madt->localApicAddr;
		iterateTables(madt, MADT_ENTRY_TYPE_LAPIC_ADDR, [&lApicAddr](madt_record_header_t* rec) {
			lApicAddr = ((MADT_LAPICOverride*) rec)->lapicAddress;
		});

		log("acpi", "lapic at %p", lApicAddr);

		// note: we iterate the table three bloody times because we need the information in a specific order
		// notably -- we need to have seen all IOAPICs before we see any IRQ mappings, because those mappings
		// modify the IOAPIC struct.


		bool foundBsp = false;
		{
			iterateTables(madt, MADT_ENTRY_TYPE_LAPIC, [&foundBsp, &lApicAddr](madt_record_header_t* rec) {

				auto lapic = (MADT_LAPIC*) rec;
				if(lapic->flags & 0x1)  // bit 0 is set if the processor is enabled.
				{
					scheduler::registerCPU(!foundBsp, lapic->processorId, lapic->apicId, lApicAddr);
					foundBsp = true;
				}

				log("acpi", "cpu %d (apic id %d)", lapic->processorId, lapic->apicId);
			});

			iterateTables(madt, MADT_ENTRY_TYPE_IOAPIC, [](madt_record_header_t* rec) {

				auto ioapic = (MADT_IOAPIC*) rec;

				device::ioapic::IOAPIC ioa;
				ioa.id = ioapic->id;
				ioa.baseAddr = (addr_t) ioapic->ioApicAddress;
				ioa.gsiBase = ioapic->globalSysInterruptBase;

				log("acpi", "ioapic[%d] at %p", ioa.id, ioa.baseAddr);

				device::ioapic::addIOAPIC(ioa);
			});

			iterateTables(madt, MADT_ENTRY_TYPE_INT_SRC, [](madt_record_header_t* rec) {
				auto intsrc = (MADT_IntSourceOverride*) rec;
				log("acpi", "intr source: bus %d, irq %d, gsi %d", intsrc->busSource, intsrc->irqSource, intsrc->globalSysInterrupt);

				device::ioapic::addISAIRQMapping(intsrc->irqSource, intsrc->globalSysInterrupt);
			});
		}



		log("acpi", "found %s, %s", util::plural("processor", scheduler::getNumCPUs()).cstr(),
			util::plural("ioapic", device::ioapic::getNumIOAPICs()).cstr());

		{
			// confirm our suspicions. we can also get the local apic base address from an MSR (0x1B).
			// it is page-aligned, so there are some flags in the lower 12 bits:
			// bit 8 is set if this is the BSP (it should be, because we have not started any APs yet)
			// bit 11 is set if the LAPIC is already enabled.

			addr_t base = cpu::readMSR(cpu::MSR_APIC_BASE);
			if(vmm::PAGE_ALIGN(base) != lApicAddr)
			{
				abort("acpi: invalid configuration, lapic base address in MSR (%p) does not match madt (%p)",
					vmm::PAGE_ALIGN(base), lApicAddr);
			}
			else if(!(base & 0x100))
			{
				abort("acpi: lapic msr says we are not the bootstrap processor??");
			}

			// testing shows that the base address we get already sets both of these flags, but just to be
			// safe we set them again. note that if the enable bit is 0, then CPUID will report the CPU as
			// *not having* the LAPIC.
			base = vmm::PAGE_ALIGN(base);
			base |= 0x800;      // this is bit 11, the enable bit
			base |= 0x100;      // this is bit 8, the BSP bit. probably not modifiable,
								// but set it just in case?

			cpu::writeMSR(cpu::MSR_APIC_BASE, base);

			// note: this 'enabling' doesn't start accepting interrupts; it just means 'not disabled'.
			// we actually enable it later.

			auto alignedBase = vmm::PAGE_ALIGN(base);

			// we need to actually map this to some virtual address -- just use the same one.
			if(vmm::allocateSpecific(alignedBase, 1) == 0)
				abort("lapic: failed to map base address %p", alignedBase);

			vmm::mapAddress(alignedBase, alignedBase, 1, vmm::PAGE_PRESENT | vmm::PAGE_WRITE);
		}
	}
}
}




















