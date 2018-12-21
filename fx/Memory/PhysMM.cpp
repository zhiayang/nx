// PhysMM.c
// Copyright (c) 2011-2013, <zhiayang@gmail.com>
// Licensed under Creative Commons Attribution 3.0 Unported.


// Implements functions to allocate physical memory.


#include "../FxLoader.hpp"
namespace Memory
{
	static FPL_t*		CurrentFPL;
	static uint32_t		FPLBottomIndex;
	static uint32_t		NumberOfFPLPairs;


	// Define a region of memory in which the VMM gets it's memory from, to create page strucures.
	static uint64_t ReservedRegionForVMM;
	static uint64_t LengthOfReservedRegion;
	static uint64_t ReservedRegionIndex = (uint64_t) -0x1000;


	uint64_t PageAlignDown(uint64_t x)
	{
		return x & ((uint64_t) ~0xFFF);
	}

	uint64_t PageAlignUp(uint64_t x)
	{
		return PageAlignDown(x + 0xFFF);
	}

	uint64_t AllocateFromReserved(uint64_t sz)
	{
		// we need this 'reserved' region especially to store page tables/directories etc.
		// if not, we'll end up with a super-screwed mapping.
		// possibly some overlap as well.

		ReservedRegionIndex += (sz * 0x1000);
		// memset((void*) (ReservedRegionForVMM + ReservedRegionIndex), 0x00, 0x1000);
		return ReservedRegionForVMM + ReservedRegionIndex;
	}


	uint64_t AllocatePage_NoMap()
	{
		if(CurrentFPL->Pairs[FPLBottomIndex].LengthInPages > 1)
		{
			CurrentFPL->Pairs[FPLBottomIndex].LengthInPages--;
			uint64_t raddr = CurrentFPL->Pairs[FPLBottomIndex].BaseAddr;

			CurrentFPL->Pairs[FPLBottomIndex].BaseAddr += 0x1000;

			// printk("[%x]\n", raddr);
			return raddr;
		}
		else if(CurrentFPL->Pairs[FPLBottomIndex].LengthInPages == 1)
		{
			uint64_t raddr = CurrentFPL->Pairs[FPLBottomIndex].BaseAddr;
			CurrentFPL->Pairs[FPLBottomIndex].BaseAddr = 0;
			CurrentFPL->Pairs[FPLBottomIndex].LengthInPages = 0;


			// Set the pointer to the next (B,L) pair.
			FPLBottomIndex++;

			return raddr;
		}
		else if(CurrentFPL->Pairs[FPLBottomIndex].BaseAddr == 0 && CurrentFPL->Pairs[FPLBottomIndex].LengthInPages == 0)
		{
			if(CurrentFPL->Pairs[FPLBottomIndex + 1].LengthInPages > 0)
				FPLBottomIndex++;

			else if(CurrentFPL->Pairs[FPLBottomIndex - 1].LengthInPages > 0)
				FPLBottomIndex--;

			return AllocatePage_NoMap();
		}

		return 0;
	}

	void FreePage_NoUnmap(uint64_t PageAddr)
	{
		for(uint32_t i = 0; i < NumberOfFPLPairs; i++)
		{
			if(PageAddr == CurrentFPL->Pairs[i].BaseAddr + (CurrentFPL->Pairs[i].LengthInPages * 0x1000))
			{
				CurrentFPL->Pairs[i].LengthInPages++;
				return;
			}

			else if(PageAddr == CurrentFPL->Pairs[i].BaseAddr - 0x1000)
			{
				CurrentFPL->Pairs[i].BaseAddr -= 0x1000;
				CurrentFPL->Pairs[i].LengthInPages++;
				return;
			}
			else if(CurrentFPL->Pairs[i].BaseAddr == 0 && CurrentFPL->Pairs[i].LengthInPages == 0)
			{
				// Try not to create a new FPL
				CurrentFPL->Pairs[i].BaseAddr = PageAddr;
				CurrentFPL->Pairs[i].LengthInPages = 1;

				NumberOfFPLPairs++;
				return;
			}
			// Doesn't matter if it's not ordered, we'll get there sooner or later.
			else
			{
				CurrentFPL->Pairs[NumberOfFPLPairs].BaseAddr = PageAddr;
				CurrentFPL->Pairs[NumberOfFPLPairs].LengthInPages = 1;

				NumberOfFPLPairs++;
				return;
			}
		}
	}















	void InitialiseFPLs(MemoryMap_type* MemoryMap)
	{
		// First, allocate a single 4K page for our FPL.
		CurrentFPL = (FPL_t*) AllocateFromReserved();

		// Make sure they're 0, so we don't get any rubbish entries. Quite a problem.
		// memset((void*) CurrentFPL, 0, 0x1000);

		NumberOfFPLPairs = 0;

		for(uint16_t i = 0; i < MemoryMap->NumberOfEntries; i++)
		{
			if(MemoryMap->Entries[i].MemoryType == G_MemoryTypeAvailable && MemoryMap->Entries[i].BaseAddress >= 0x00100000)
			{
				// It's available memory, add a (B,L) pair.
				CurrentFPL->Pairs[NumberOfFPLPairs].BaseAddr = PageAlignUp(MemoryMap->Entries[i].BaseAddress);
				CurrentFPL->Pairs[NumberOfFPLPairs].LengthInPages = (PageAlignDown(MemoryMap->Entries[i].Length) / 0x1000) - 1;

				NumberOfFPLPairs++;
			}
		}
		FPLBottomIndex = 0;

		// The bottom-most FPL would usually be from 1MB up.
		// However, we must set it to the top of our kernel.

		// HACK: set it to 8 mb -- this sets a limit on our kernel being no bigger than about 6 mb.

		uint64_t OldBaseAddr = CurrentFPL->Pairs[0].BaseAddr;

		CurrentFPL->Pairs[0].BaseAddr = PageAlignUp((uint64_t) 0x00800000 + (uint64_t) CurrentFPL);
		CurrentFPL->Pairs[0].LengthInPages = CurrentFPL->Pairs[0].LengthInPages - ((CurrentFPL->Pairs[0].BaseAddr - OldBaseAddr) / 0x1000);
	}




	uint64_t AllocatePage()
	{
		// This one allocates AND maps pages.

		uint64_t p = AllocatePage_NoMap();
		MapAddress(p, p, 0x03);

		// Do us a favour and zero the memory first.
		memset((void*) p, 0x00, 0x1000);

		return p;
	}

	void FreePage(uint64_t PageAddr)
	{
		FreePage_NoUnmap(PageAddr);

		// FIXME: This assumes that the memory to free is 1-1 mapped.
		UnmapAddress(PageAddr);
	}



	void InitialisePhys(MemoryMap_type* map)
	{
		// See HAL_InitialiseFPLs() for an in-depth explanation on this
		// FPL system.

		ReservedRegionForVMM = PageAlignUp(KernelEnd);
		LengthOfReservedRegion = 0x00300000;

		InitialiseFPLs(map);
	}
}



















