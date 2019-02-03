// acpi.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "devices/acpi.h"

#include "efi/system-table.h"

namespace nx {
namespace acpi
{
	bool checkTable(header_t* table)
	{
		auto raw = (uint8_t*) table;

		uint8_t sum = 0;
		for(size_t i = 0; i < table->length; i++)
			sum += raw[i];

		return sum == 0;
	}

	static void readTable(header_t* table)
	{
		if(strncmp(table->signature, "APIC", 4) == 0)
		{
			readMADT((MADTable*) table);
		}
		else if(strncmp(table->signature, "HPET", 4) == 0)
		{
			// println("found hpet");
		}
	}













	void init(BootInfo* bi)
	{
		// grab all the acpi entries in the memory map -- presumably all the config tables
		// are indicated as such!!!

		for(uint64_t i = 0; i < bi->mmEntryCount; i++)
		{
			auto ent = &bi->mmEntries[i];
			if(krt::match(ent->memoryType, MemoryType::ACPI, MemoryType::EFIRuntimeCode, MemoryType::EFIRuntimeData))
			{
				if(vmm::allocateSpecific(ent->address, ent->numPages) == 0)
					abort("apic::init(): failed to map ACPI memory at %p", ent->address);

				// ok, now just map it in an identity fashion.
				vmm::mapAddress(ent->address, ent->address, ent->numPages, vmm::PAGE_PRESENT);
			}
		}


		auto st = (efi_system_table*) bi->efiSysTable;
		auto ct = st->ConfigurationTable;


		{
			efi_guid rsdp_guid = ACPI_TABLE_GUID;
			efi_guid rsdp2_guid = ACPI_20_TABLE_GUID;

			void* rsdp = 0;
			void* rsdp2 = 0;

			for(size_t i = 0; i < st->NumberOfTableEntries; i++)
			{
				auto ent = &ct[i];
				if(memcmp(&rsdp_guid.data[0], &ent->VendorGuid.data[0], 16) == 0)
					rsdp = ent->VendorTable;

				else if(memcmp(&rsdp2_guid.data[0], &ent->VendorGuid.data[0], 16) == 0)
					rsdp2 = ent->VendorTable;
			}

			// prefer the rsdp version 2
			if(rsdp2)
			{
				auto table = (RSDPointer2*) rsdp2;
				if(strncmp(table->version1.signature, "RSD PTR ", 8) != 0)
					abort("invalid rsdp: signature '%.8s' mismatch", table->version1.signature);

				auto xsdt = (XSDTable*) table->xsdtAddr;
				size_t numHdrs = (xsdt->header.length - sizeof(header_t)) / sizeof(uint64_t);

				log("acpi", "reading xsdt (%zu tables)", numHdrs);
				for(size_t i = 0; i < numHdrs; i++)
					readTable((header_t*) xsdt->tables[i]);
			}
			else if(rsdp)
			{
				auto table = (RSDPointer*) rsdp;
				if(strncmp(table->signature, "RSD PTR ", 8) != 0)
					abort("invalid rsdp: signature '%.8s' mismatch", table->signature);

				auto rsdt = (RSDTable*) (addr_t) table->rsdtAddr;
				size_t numHdrs = (rsdt->header.length - sizeof(header_t)) / sizeof(uint32_t);

				log("acpi", "reading rsdt (%zu tables)", numHdrs);
				for(size_t i = 0; i < numHdrs; i++)
					readTable((header_t*) (addr_t) rsdt->tables[i]);
			}
			else
			{
				abort("RSDP not found; system likely does not support ACPI!");
			}
		}

		log("acpi", "finished parsing acpi tables");
	}
}
}

























