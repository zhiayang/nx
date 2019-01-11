// apic.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "devices/acpi.h"
#include "devices/x64/apic.h"

namespace nx {
namespace apic
{
	static constexpr uint8_t MADT_ENTRY_TYPE_LAPIC      = 0;
	static constexpr uint8_t MADT_ENTRY_TYPE_IOAPIC     = 1;
	static constexpr uint8_t MADT_ENTRY_TYPE_INT_SRC    = 2;
	static constexpr uint8_t MADT_ENTRY_TYPE_NMI        = 4;
	static constexpr uint8_t MADT_ENTRY_TYPE_LAPIC_ADDR = 5;


	void init(acpi::MADTable* madt)
	{
		using namespace nx::acpi;

		if(!checkTable((header_t*) madt))
		{
			println("acpi/madt: invalid checksum, skipping");
			return;
		}

		size_t len = madt->header.length;
		size_t ofs = sizeof(MADTable);

		println("acpi/madt: %d bytes", madt->header.length);

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


		while(ofs < len)
		{
			auto rec = (madt_record_header_t*) ((addr_t) madt + ofs);
			if(rec->type == MADT_ENTRY_TYPE_LAPIC)
			{
				auto lapic = (MADT_LAPIC*) rec;
				if(lapic->flags & 0x1)
					scheduler::registerProcessor(lapic->processorId, lapic->apicId, lApicAddr);
			}
			else if(rec->type == MADT_ENTRY_TYPE_IOAPIC)
			{
				auto ioapic = (MADT_IOAPIC*) rec;
			}

			ofs += rec->length;
		}
	}
}














namespace interrupts
{
	void init(BootInfo* bi)
	{
	}
}
}