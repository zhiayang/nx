// apic.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

namespace nx
{
	namespace acpi
	{
		struct MADTable;
	}

	namespace device
	{
		namespace apic
		{
			struct IOAPIC
			{
				int id;

				// the selector is at the base address,
				// the actual value register is at baseAddr + 0x10
				addr_t baseAddr;

				// no idea what this does.
				addr_t gsiBase;

				// the number of IRQs that this ioapic can handle
				int maxRedirections;
			};


			void preinit();
			bool init();

			void writeIOAPIC(IOAPIC* ioapic, uint32_t reg, uint32_t value);
			uint32_t readIOAPIC(IOAPIC* ioapic, uint32_t reg);

			void addIOAPIC(const IOAPIC& ioa);
			size_t getNumIOAPICs();

			void maskIRQ(int num);
			void unmaskIRQ(int num);

			void sendEOI(int num);
		}
	}
}