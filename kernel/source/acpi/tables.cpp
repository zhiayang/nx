// acpi.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "devices/acpi.h"

#include "efi/system-table.h"

#define NX_BOOTINFO_VERSION NX_MAX_BOOTINFO_VERSION
#include "bootinfo.h"

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
		if(strncmp(table->signature, MADTable::name, 4) == 0)
		{
			readMADT((MADTable*) table);
		}
		else if(strncmp(table->signature, FADTable::name, 4) == 0)
		{
			// println("found hpet");
		}

		// todo: bit 1 in a >= v3 FADT tells if we have an 8042 ps/2 controller
	}









	static void* findTablesFromUEFI(BootInfo* bi, bool* isXSDT)
	{
		assert(bi->flags & BOOTINFO_FLAG_UEFI);

		void* rsdp = 0;
		void* xsdp = 0;

		auto st = (efi_system_table*) bi->efi.efiSysTable;
		auto ct = st->ConfigurationTable;
		{
			efi_guid rsdp_guid = ACPI_TABLE_GUID;
			efi_guid rsdp2_guid = ACPI_20_TABLE_GUID;


			for(size_t i = 0; i < st->NumberOfTableEntries; i++)
			{
				auto ent = &ct[i];
				if(memcmp(&rsdp_guid.data[0], &ent->VendorGuid.data[0], 16) == 0)
					rsdp = ent->VendorTable;

				else if(memcmp(&rsdp2_guid.data[0], &ent->VendorGuid.data[0], 16) == 0)
					xsdp = ent->VendorTable;
			}
		}

		if(xsdp) *isXSDT = true;

		auto table = (RSDPointer2*) (xsdp ? xsdp : rsdp);
		if(strncmp(table->version1.signature, "RSD PTR ", 8) != 0)
			abort("invalid rsdp: signature '{.8}' mismatch", table->version1.signature);

		assert(!isXSDT || table->version1.revision > 1);

		if(isXSDT && table->version1.revision > 1)
		{
			return (void*) table->xsdtAddr;
		}
		else
		{
			return (void*) (addr_t) table->version1.rsdtAddr;
		}
	}


	static void* findTablesFromBIOS(BootInfo* bi, bool* isXSDT)
	{
		assert(bi->flags & BOOTINFO_FLAG_BIOS);

		auto hdr = (header_t*) bi->bios.acpiTable;

		if(strncmp(hdr->signature, "RSDT", 4) == 0)
		{
			return hdr;
		}
		else if(strncmp(hdr->signature, "XSDT", 4) == 0)
		{
			*isXSDT = true;
			return hdr;
		}
		else
		{
			abort("invalid rsdt/xsdt: signature '{.4}' mismatch", hdr->signature);
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
				auto ea = VirtAddr(ent->address);
				if(vmm::allocateSpecific(ea, ent->numPages).isZero())
					abort("apic::init(): failed to map ACPI memory at {p}", ent->address);

				// ok, now just map it in an identity fashion.
				vmm::mapAddress(ea, ea.physIdentity(), ent->numPages, vmm::PAGE_PRESENT);
			}
		}

		void* table = 0;
		bool isXSDT = false;
		if(bi->flags & BOOTINFO_FLAG_BIOS)      table = findTablesFromBIOS(bi, &isXSDT);
		else if(bi->flags & BOOTINFO_FLAG_UEFI) table = findTablesFromUEFI(bi, &isXSDT);

		if(!table)
		{
			abort("RSDP not found; system likely does not support ACPI!");
		}

		if(isXSDT)
		{
			auto xsdt = (XSDTable*) table;
			size_t numHdrs = (xsdt->header.length - sizeof(header_t)) / sizeof(uint64_t);

			log("acpi", "reading xsdt ({} tables)", numHdrs);
			for(size_t i = 0; i < numHdrs; i++)
				readTable((header_t*) xsdt->tables[i]);
		}
		else
		{
			auto rsdt = (RSDTable*) table;
			size_t numHdrs = (rsdt->header.length - sizeof(header_t)) / sizeof(uint32_t);

			log("acpi", "reading rsdt ({} tables)", numHdrs);
			for(size_t i = 0; i < numHdrs; i++)
				readTable((header_t*) (addr_t) rsdt->tables[i]);
		}

		log("acpi", "finished parsing acpi tables");
	}
}
}

























