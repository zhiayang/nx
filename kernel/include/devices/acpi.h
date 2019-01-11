// acpi.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

namespace nx
{
	struct BootInfo;
	namespace acpi
	{
		struct header_t;

		void init(BootInfo* bi);
		bool checkTable(header_t* table);

		struct RSDPointer
		{
			char        signature[8];
			uint8_t     checksum;
			char        oemId[6];
			uint8_t     revision;
			uint32_t    rsdtAddr;

		} __attribute__((packed));

		struct RSDPointer2
		{
			RSDPointer  version1;
			uint32_t    length;
			uint64_t    xsdtAddr;
			uint8_t     extendedChecksum;
			uint8_t     reserved[3];

		} __attribute__((packed));


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

		struct RSDTable
		{
			header_t    header;
			uint32_t    tables[];

		} __attribute__((packed));

		struct XSDTable
		{
			header_t    header;
			uint64_t    tables[];

		} __attribute__((packed));






		struct madt_record_header_t
		{
			uint8_t     type;
			uint8_t     length;
		} __attribute__((packed));

		struct MADTable
		{
			header_t    header;
			uint32_t    localApicAddr;
			uint32_t    flags;

		} __attribute__((packed));

		struct MADT_LAPIC
		{
			madt_record_header_t header;
			uint8_t     processorId;
			uint8_t     apicId;
			uint32_t    flags;

		} __attribute__((packed));

		struct MADT_IOAPIC
		{
			madt_record_header_t header;
			uint8_t     id;
			uint8_t     reserved;
			uint32_t    ioApicAddress;
			uint32_t    globalSysInterruptBase;
		} __attribute__((packed));

		struct MADT_IntSourceOverride
		{
			madt_record_header_t header;
			uint8_t     busSource;
			uint8_t     irqSource;
			uint32_t    globalSysInterrupt;
			uint16_t    flags;

		} __attribute__((packed));

		struct MADT_LAPICOverride
		{
			madt_record_header_t header;
			uint16_t    reserved;
			uint64_t    lapicAddress;

		} __attribute__((packed));
	}
}




















