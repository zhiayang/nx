// main.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include "FxLoader.hpp"
#include "ELF.hpp"

#include "heatshrink/heatshrink_decoder.h"

extern "C" uint64_t StartBSS;
extern "C" uint64_t EndBSS;

uint64_t KernelEnd = 0x00400000;

// set in start.s
static uint64_t CurrentCR3 = 0x3000;

extern "C" uint64_t LoaderBootstrap(uint64_t MultibootMagic, uint64_t MBTAddr)
{
	// clear the bss
	{
		uint64_t bs = (uint64_t) &StartBSS;
		uint64_t be = (uint64_t) &EndBSS;
		uint64_t sz = be - bs;

		memset((void*) bs, 0, sz);
	}

	Console::Initialise();

	Console::Print("[fx] loader is initialising...\n");
	assert(MultibootMagic == 0x2BADB002);

	Memory::MemoryMap_type MemoryMap;

	Console::Print("Multiboot struct at %x\n", MBTAddr);

	Memory::Initialise((Multiboot::Info_type*) (uint64_t) MBTAddr, &MemoryMap);
	Console::Print("Memory map initialised\n");

	uint64_t compressedAddr = 0;
	uint64_t compressedLength = 0;
	LoadKernelModule((Multiboot::Info_type*) (uint64_t) MBTAddr, &compressedAddr, &compressedLength);

	Memory::InitialiseVirt();
	Console::Print("Virtual memory online\n");

	Memory::InitialisePhys(&MemoryMap);
	Console::Print("Physical memory online\n");

	Memory::Initialise();
	Console::Print("Dynamic memory online\n");

	uint64_t kernelAddress = 0xFFFFFF3000000000;
	uint32_t kernelLength = 0;


	{
		// get length as first 8 bytes, endian swapped.
		kernelLength = (uint32_t) __builtin_bswap64(*((uint64_t*) compressedAddr));

		for(uint64_t i = 0; i < (kernelLength + 0xFFF) / 0x1000; i++)
		{
			uint64_t p = Memory::AllocatePage();
			Memory::MapAddress(kernelAddress + (i * 0x1000), p, 0x03);
		}

		compressedAddr += 8;
		compressedLength -= 8;

		Console::Print("Decompressing kernel64: compressed %d bytes\n", compressedLength);

		// reference makefile line 96, -w and -l option respectively.
		heatshrink_decoder* decoder = heatshrink_decoder_alloc(0x1000, 14 /* -w */, 6 /* -l */);

		size_t readBytes = 0;	// such grammar
		size_t wrote = 0;
		while(readBytes < compressedLength && wrote < kernelLength)
		{
			size_t curRead = 0;
			heatshrink_decoder_sink(decoder, (uint8_t*) (compressedAddr + readBytes), compressedLength - readBytes, &curRead);
			readBytes += curRead;

			// if(res == HSDR_SINK_FULL)
			{
				size_t curWrote = 0;
				heatshrink_decoder_poll(decoder, (uint8_t*) (kernelAddress + wrote), kernelLength - wrote, &curWrote);
				wrote += curWrote;

				// Console::Print("\r                       \r(%d/%d)\n", wrote, kernelLength);
				Console::Print(".");
			}
		}

		auto finish_res = heatshrink_decoder_finish(decoder);
		if(finish_res == HSDR_FINISH_MORE)
		{
			while(wrote < kernelLength)
			{
				size_t curWrote = 0;
				heatshrink_decoder_poll(decoder, (uint8_t*) (kernelAddress + wrote), kernelLength - wrote, &curWrote);
				wrote += curWrote;

				// Console::Print("\r                       \r(%d/%d)\n", wrote, kernelLength);
				Console::Print(".");
			}
		}

		Console::Print("\nDecompressed: %d bytes\n", kernelLength);
	}


	Console::Print("Loading kernel...\n");
	uint64_t entry = LoadKernelELF(kernelAddress, kernelLength);



	// 'compute' the version number from the build number.
	{
		uint64_t VER_MAJOR = 5;
		uint64_t VER_MINOR = 0;
		uint64_t VER_REVSN = 0;

		Console::Print("Loaded orionx-compatible kernel: version %d.%d.%d\n", VER_MAJOR, VER_MINOR, VER_REVSN);
	}


	Console::Print("Entry point: %x\n", entry);
	Console::Print("Handling control to kernel\n--------------------------\n\n");


	// bottom 32 bits: entry point.
	// next 8 bits: cursor X
	// next 8 bits: cursor Y
	// top 16 bits: nothing

	uint64_t cx = Console::CursorX;
	uint64_t cy = Console::CursorY;

	return (cy << 40) | (cx << 32) | (entry & 0xFFFFFFFF);
}


void LoadKernelModule(Multiboot::Info_type* mbt, uint64_t* start, uint64_t* length)
{
	assert(mbt);
	assert(mbt->mods_count == 1 && "Expected only one module, the [mx] kernel!");

	Multiboot::Module_type* mod = (Multiboot::Module_type*) (uint64_t) mbt->mods_addr;
	Console::Print("mods_addr: %x\n", mbt->mods_addr);
	Console::Print("[mx] kernel executable loaded at %x, %d bytes long\n", mod->ModuleStart, mod->ModuleEnd - mod->ModuleStart);

	KernelEnd = Memory::PageAlignUp(mod->ModuleEnd);

	*start = mod->ModuleStart;
	*length = mod->ModuleEnd - mod->ModuleStart;
}

uint64_t LoadKernelELF(uint64_t start, uint64_t length)
{
	(void) length;

	ELF64FileHeader_type* FileHeader = (ELF64FileHeader_type*) start;
	assert(FileHeader->ElfIdentification[EI_MAGIC0] == ELF_MAGIC0);
	assert(FileHeader->ElfIdentification[EI_MAGIC1] == ELF_MAGIC1);
	assert(FileHeader->ElfIdentification[EI_MAGIC2] == ELF_MAGIC2);
	assert(FileHeader->ElfIdentification[EI_MAGIC3] == ELF_MAGIC3);
	assert(FileHeader->ElfIdentification[EI_CLASS] == ElfClass64Bit);
	assert(FileHeader->ElfIdentification[EI_DATA] == ElfDataLittleEndian);
	assert(FileHeader->ElfType == ElfTypeExecutable);

	for(uint64_t k = 0; k < FileHeader->ElfProgramHeaderEntries; k++)
	{
		Console::Print("Loading segment %d\n", k);
		ELF64ProgramHeader_type* ProgramHeader = (ELF64ProgramHeader_type*) (start + FileHeader->ElfProgramHeaderOffset + (k * FileHeader->ElfProgramHeaderEntrySize));

		if(ProgramHeader->ProgramType == ProgramTypeNull || ProgramHeader->ProgramMemorySize == 0 || ProgramHeader->ProgramVirtualAddress == 0)
			continue;

		for(uint64_t m = 0; m < (ProgramHeader->ProgramMemorySize + 0x1000) / 0x1000; m++)
		{
			uint64_t actualvirt = (ProgramHeader->ProgramVirtualAddress + (m * 0x1000)) & ((uint64_t) ~0xFFF);

			uint64_t t = Memory::AllocateFromReserved();
			Memory::MapAddress(actualvirt, t, 0x03);
		}


		memcpy((void*) (ProgramHeader->ProgramVirtualAddress), (void*) (start + ProgramHeader->ProgramOffset), ProgramHeader->ProgramFileSize);

		if(ProgramHeader->ProgramMemorySize > ProgramHeader->ProgramFileSize)
		{
			memset((void*) (ProgramHeader->ProgramVirtualAddress + ProgramHeader->ProgramFileSize), 0,
				ProgramHeader->ProgramMemorySize - ProgramHeader->ProgramFileSize);
		}
	}

	return FileHeader->ElfEntry;
}


namespace Memory
{
	uint64_t GetCurrentCR3()
	{
		return CurrentCR3;
	}
}







