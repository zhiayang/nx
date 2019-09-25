// acpi/tables.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.


#pragma once

#include <stddef.h>
#include "stdint.h"

namespace nx
{
	namespace acpi
	{
		struct header_t
		{
			char        signature[4];
			uint32_t    length;
			uint8_t     revision;
			uint8_t     checksum;
			char        oemId[6];
			char        oemTableId[8];
			uint32_t    oemRevision;
			uint32_t    creatorId;
			uint32_t    creatorRevision;

		} __attribute__((packed));

		struct generic_address_t
		{
			uint8_t     addressSpace;
			uint8_t     bitWidth;
			uint8_t     bitOffset;
			uint8_t     accessWidth;
			uint64_t    address;
		};

		struct FADTable
		{
			static constexpr const char* name = "FACP";

			uint32_t            facsAddress;            // 32-bit physical address of FACS
			uint32_t            dsdtAddress;            // 32-bit physical address of DSDT
			uint8_t             reserved1;              // System Interrupt Model (ACPI 1.0) - not used in ACPI 2.0+
			uint8_t             preferredPMProfile;     // Conveys preferred power management profile to OSPM.
			uint16_t            sciInterrupt;           // System vector of SCI interrupt
			uint32_t            sciCommandPort;         // 32-bit Port address of SMI command port
			uint8_t             acpiEnableValue;        // Value to write to SMI_CMD to enable ACPI
			uint8_t             acpiDisableValue;       // Value to write to SMI_CMD to disable ACPI
			uint8_t             s4BiosRequestValue;     // Value to write to SMI_CMD to enter S4BIOS state
			uint8_t             perfStateControlValue;  // Processor performance state control
			uint32_t            PM1aEventBlock;         // 32-bit port address of Power Mgt 1a Event Reg Blk
			uint32_t            PM1bEventBlock;         // 32-bit port address of Power Mgt 1b Event Reg Blk
			uint32_t            PM1aControlBlock;       // 32-bit port address of Power Mgt 1a Control Reg Blk
			uint32_t            PM1bControlBlock;       // 32-bit port address of Power Mgt 1b Control Reg Blk
			uint32_t            PM2ControlBlock;        // 32-bit port address of Power Mgt 2 Control Reg Blk
			uint32_t            PMTimerBlock;           // 32-bit port address of Power Mgt Timer Ctrl Reg Blk
			uint32_t            GPE0Block;              // 32-bit port address of General Purpose Event 0 Reg Blk
			uint32_t            GPE1Block;              // 32-bit port address of General Purpose Event 1 Reg Blk
			uint8_t             PM1EventLength;         // Byte Length of ports at Pm1xEventBlock
			uint8_t             PM1ControlLength;       // Byte Length of ports at Pm1xControlBlock
			uint8_t             PM2ControlLength;       // Byte Length of ports at Pm2ControlBlock
			uint8_t             PMTimerLength;          // Byte Length of ports at PmTimerBlock
			uint8_t             GPE0BlockLength;        // Byte Length of ports at Gpe0Block
			uint8_t             GPE1BlockLength;        // Byte Length of ports at Gpe1Block
			uint8_t             GPE1Base;               // Offset in GPE number space where GPE1 events start
			uint8_t             CSTControl;             // Support for the _CST object and C-States change notification
			uint16_t            PLevel2Latency;         // Worst case HW latency to enter/exit C2 state
			uint16_t            PLevel3Latency;         // Worst case HW latency to enter/exit C3 state
			uint16_t            FlushSize;              // Processor memory cache line width, in bytes
			uint16_t            FlushStride;            // Number of flush strides that need to be read
			uint8_t             DutyOffset;             // Processor duty cycle index in processor P_CNT reg
			uint8_t             DutyWidth;              // Processor duty cycle value bit width in P_CNT register
			uint8_t             DayAlarm;               // Index to day-of-month alarm in RTC CMOS RAM
			uint8_t             MonthAlarm;             // Index to month-of-year alarm in RTC CMOS RAM
			uint8_t             Century;                // Index to century in RTC CMOS RAM
			uint16_t            IAPCBootFlags;          // IA-PC Boot Architecture Flags (see below for individual flags)
			uint8_t             reserved2;              // Reserved, must be zero
			uint32_t            flags;                  // Miscellaneous flag bits (see below for individual flags)
			generic_address_t   resetRegister;          // 64-bit address of the Reset register
			uint8_t             resetValue;             // Value to write to the ResetRegister port to reset the system
			uint16_t            ARMBootFlags;           // ARM-Specific Boot Flags (see below for individual flags) (ACPI 5.1)
			uint8_t             minorVersion;           // FADT Minor Revision (ACPI 5.1)
			uint64_t            x_facsAddress;          // 64-bit physical address of FACS
			uint64_t            x_dsdtAddress;          // 64-bit physical address of DSDT
			generic_address_t   x_PM1aEventBlock;       // 64-bit Extended Power Mgt 1a Event Reg Blk address
			generic_address_t   x_PM1bEventBlock;       // 64-bit Extended Power Mgt 1b Event Reg Blk address
			generic_address_t   x_PM1aControlBlock;     // 64-bit Extended Power Mgt 1a Control Reg Blk address
			generic_address_t   x_PM1bControlBlock;     // 64-bit Extended Power Mgt 1b Control Reg Blk address
			generic_address_t   x_PM2ControlBlock;      // 64-bit Extended Power Mgt 2 Control Reg Blk address
			generic_address_t   x_PMTimerBlock;         // 64-bit Extended Power Mgt Timer Ctrl Reg Blk address
			generic_address_t   x_GPE0Block;            // 64-bit Extended General Purpose Event 0 Reg Blk address
			generic_address_t   x_GPE1Block;            // 64-bit Extended General Purpose Event 1 Reg Blk address
			generic_address_t   sleepControlRegister;   // 64-bit Sleep Control register (ACPI 5.0)
			generic_address_t   sleepStatisRegister;    // 64-bit Sleep Status register (ACPI 5.0)
			uint64_t            hypervisorVendor;       // Hypervisor Vendor ID (ACPI 6.0)

		} __attribute__((packed));
	}
}
