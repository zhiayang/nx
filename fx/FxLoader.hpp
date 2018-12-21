// FxLoader.hpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#pragma once

namespace IOPort
{
	uint8_t ReadByte(uint16_t Port);
	uint16_t Read16(uint16_t Port);
	uint32_t Read32(uint16_t Port);
	void WriteByte(uint16_t Port, uint8_t Value);
	void Write16(uint16_t Port, uint16_t Value);
	void Write32(uint16_t Port, uint32_t Value);
	uint64_t ReadMSR(uint32_t msr);
	void WriteMSR(uint32_t msr, uint64_t val);
}

namespace Console
{
	void Initialise();
	void ClearScreen(uint8_t Colour);
	void ScrollUp();

	void Print(const char* string, va_list args);
	void Print(const char* string, ...);

	extern uint8_t CursorX;
	extern uint8_t CursorY;
}


namespace Util
{
	char* ConvertToString(int64_t num, char* out);
	int64_t ConvertToInt(char* str, uint8_t base);
	void DumpBytes(uint64_t address, uint64_t length);
}

namespace Multiboot
{
	// The Multiboot information.
	struct __attribute__((packed)) Info_type
	{
		uint32_t flags, mem_lower, mem_upper, boot_device, cmdline, mods_count, mods_addr;
		uint32_t num, size, addr, shndx;
		uint32_t mmap_length, mmap_addr;
	};

	struct Module_type
	{
		uint32_t ModuleStart;
		uint32_t ModuleEnd;
		uint32_t string;
		uint32_t reserved;

	} __attribute__((packed));

	struct MemoryMap_type
	{
		uint32_t Size;
		uint32_t BaseAddr_Low;
		uint32_t BaseAddr_High;
		uint32_t Length_Low;
		uint32_t Length_High;
		uint32_t Type;

	} __attribute__((packed));
}

namespace Memory
{
	#define G_MemoryTypeAvailable		1
	#define G_MemoryTypeReserved		2
	#define G_MemoryACPI				3
	#define G_MemoryNVS					4
	#define G_MemoryBadRam				5

	struct MemoryMapEntry_type
	{
		uint64_t BaseAddress;
		uint64_t Length;
		uint8_t MemoryType;
	};

	struct MemoryMap_type
	{
		uint16_t NumberOfEntries;
		uint32_t SizeOfThisStructure;

		MemoryMapEntry_type Entries[512];
	};

	struct BLPair_t
	{
		uint64_t BaseAddr;
		uint64_t LengthInPages;
	};

	struct FPL_t
	{
		BLPair_t Pairs[256];
	};

	struct PageMapStructure
	{
		uint64_t Entry[512];
	};

	uint64_t PageAlignDown(uint64_t x);
	uint64_t PageAlignUp(uint64_t x);

	void Initialise(Multiboot::Info_type* MBTStruct, MemoryMap_type* map);
	void InitialisePhys(MemoryMap_type* map);
	void InitialiseVirt();

	uint64_t AllocateFromReserved(uint64_t sz = 1);
	uint64_t AllocatePage_NoMap();
	void FreePage_NoUnmap(uint64_t PageAddr);

	uint64_t AllocatePage();
	void FreePage(uint64_t PageAddr);

	void MapAddress(uint64_t VirtAddr, uint64_t PhysAddr, uint64_t Flags, PageMapStructure* PML4);
	void MapAddress(uint64_t VirtAddr, uint64_t PhysAddr, uint64_t Flags);

	void UnmapAddress(uint64_t VirtAddr, PageMapStructure* PML4);
	void UnmapAddress(uint64_t VirtAddr);

	uint64_t GetCurrentCR3();



	void Initialise();
	extern "C" void* malloc(uint64_t s);
	extern "C" void free(void* Pointer);
	extern "C" void* realloc(void* ptr, uint64_t size);
}

extern "C" uint64_t LoaderBootstrap(uint64_t magic, uint64_t mbt);
void LoadKernelModule(Multiboot::Info_type* mbt, uint64_t* start, uint64_t* length);
uint64_t LoadKernelELF(uint64_t start, uint64_t length);




extern uint64_t KernelEnd;


extern "C" void* memset(void* destptr, int value, uint64_t length);
extern "C" void* memcpy(void* dest, const void* src, uint64_t len);
extern "C" void* memmove(void* dst, const void* src, uint64_t n);
extern "C" size_t strlen(const char* str);



extern "C" void __assert(const char* filename, unsigned long line, const char* function_name, const char* expression);

/* Otherwise, declare the normal assert macro. */
#define assert(invariant)	((invariant) ? (void) (0) : __assert(__FILE__, __LINE__, __PRETTY_FUNCTION__, #invariant))







