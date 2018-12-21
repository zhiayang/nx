// MemoryMap.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../FxLoader.hpp"

namespace Memory
{
	void Initialise(Multiboot::Info_type* MBTStruct, MemoryMap_type* memmap)
	{
		uint64_t TotalMapAddressess = (uint64_t) MBTStruct->mmap_addr;
		uint64_t TotalMapLength = MBTStruct->mmap_length;

		memmap->SizeOfThisStructure = sizeof(uint32_t) + sizeof(uint16_t);
		memmap->NumberOfEntries = 0;


		// Check if the fields are valid:
		if(!(MBTStruct->flags & (1 << 6)))
		{
			Console::Print("FLAGS: %x", MBTStruct->flags);
			assert("No Multiboot Memory Map!" && 0);
		}

		Multiboot::MemoryMap_type* mmap = (Multiboot::MemoryMap_type*) TotalMapAddressess;
		while((uint64_t) mmap < (uint64_t)(TotalMapAddressess + TotalMapLength))
		{

			(memmap->Entries[memmap->NumberOfEntries]).BaseAddress = (uint64_t) mmap->BaseAddr_Low | (uint64_t)(mmap->BaseAddr_High) << 32;
			(memmap->Entries[memmap->NumberOfEntries]).Length = (uint64_t)(mmap->Length_Low | (uint64_t)(mmap->Length_High) << 32);
			(memmap->Entries[memmap->NumberOfEntries]).MemoryType = (uint8_t) mmap->Type;
			memmap->SizeOfThisStructure += sizeof(MemoryMapEntry_type);

			Console::Print("Found memory map entry: %x -> %x, %d\n", (memmap->Entries[memmap->NumberOfEntries]).BaseAddress, (memmap->Entries[memmap->NumberOfEntries]).Length, mmap->Type);

			memmap->NumberOfEntries++;
			mmap = (Multiboot::MemoryMap_type*)((uint64_t) mmap + mmap->Size + sizeof(uint32_t));
		}
	}
}




