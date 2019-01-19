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
				int gsiBase;

				// the number of IRQs that this ioapic can handle
				int maxRedirections;
			};


			void preinit();
			bool init();

			void addIOAPIC(const IOAPIC& ioa);
			size_t getNumIOAPICs();

			void maskIRQ(int num);
			void unmaskIRQ(int num);

			// sets up the ioapic to trigger interrupt 'vector' on the cpu with id 'apicId', when it receives irq 'irq' from a device.
			void setInterrupt(int irq, int vector, int apicId);

			void sendEOI(int num);
		}
	}
}