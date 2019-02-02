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

			// sets up the ioapic to trigger interrupt 'vector' on the cpu with id 'apicId',
			// when it receives 'irq' from a device.
			void setInterrupt(int irq, int vector, int apicId);

			// send an EOI for the irq.
			void sendEOI(int num);

			// sets the pin mapping from the legacy ISA interrupt number to the IOAPIC interrupt number.
			// uses the intr source info in the MADT.
			void addISAIRQMapping(int isa, int irq);

			// same as the above, but getting the mapping. if there is no mapping, it returns -1
			int getISAIRQMapping(int isa);
		}
	}
}

















