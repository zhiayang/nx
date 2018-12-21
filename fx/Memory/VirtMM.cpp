// VirtMM.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../FxLoader.hpp"

namespace Memory
{
	static PageMapStructure* CurrentPML4T;
	static bool IsPaging;

	#define I_RECURSIVE_SLOT	510

	// Convert an address into array index of a structure
	// E.G. int index = I_PML4_INDEX(0xFFFFFFFFFFFFFFFF); // index = 511
	#define I_PML4_INDEX(addr)		((((uintptr_t)(addr)) >> 39) & 0x1FF)
	#define I_PDPT_INDEX(addr)		((((uintptr_t)(addr)) >> 30) & 0x1FF)
	#define I_PD_INDEX(addr)		((((uintptr_t)(addr)) >> 21) & 0x1FF)
	#define I_PT_INDEX(addr)		((((uintptr_t)(addr)) >> 12) & 0x1FF)


	#define I_Present		(0x1)
	#define I_ReadWrite		(0x2)
	#define I_AlignMask		((uint64_t) ~0xFFF)

	void InitialiseVirt()
	{
		PageMapStructure* OriginalPml4 = (PageMapStructure*) GetCurrentCR3();

		OriginalPml4->Entry[I_RECURSIVE_SLOT] = (uint64_t) OriginalPml4 | I_Present | I_ReadWrite;
		CurrentPML4T = OriginalPml4;
		IsPaging = true;
	}

	void ChangeRawCR3(uint64_t newval)
	{
		asm volatile("mov %[nv], %%rax; mov %%rax, %%cr3" :: [nv]"g"(newval) : "memory", "rax");
	}

	uint64_t GetRawCR3()
	{
		uint64_t ret = 0;
		asm volatile("mov %%cr3, %%rax; mov %%rax, %[r]" : [r]"=g"(ret) :: "memory", "rax");

		return ret;
	}

	void SwitchPML4T(PageMapStructure* PML4T)
	{
		CurrentPML4T = PML4T;
	}

	PageMapStructure* GetCurrentPML4T()
	{
		return CurrentPML4T;
	}

	void invlpg(PageMapStructure* p)
	{
		asm volatile("invlpg (%0)" : : "a" ((uint64_t) p));
	}








	void MapAddress(uint64_t VirtAddr, uint64_t PhysAddr, uint64_t Flags, PageMapStructure* PML4)
	{
		uint64_t PageTableIndex					= I_PT_INDEX(VirtAddr);
		uint64_t PageDirectoryIndex				= I_PD_INDEX(VirtAddr);
		uint64_t PageDirectoryPointerTableIndex	= I_PDPT_INDEX(VirtAddr);
		uint64_t PML4TIndex						= I_PML4_INDEX(VirtAddr);

		assert(PageTableIndex < 512);
		assert(PageDirectoryIndex < 512);
		assert(PageDirectoryPointerTableIndex < 512);
		assert(PML4TIndex < 512);

		PageMapStructure* PML = (PML4 == 0 ? GetCurrentPML4T() : PML4);
		bool other = (PML != GetCurrentPML4T());

		assert(PML);

		if(other)
			MapAddress((uint64_t) PML, (uint64_t) PML, 0x7);



		if(!(PML->Entry[PML4TIndex] & I_Present))
		{
			PML->Entry[PML4TIndex] = Memory::AllocateFromReserved() | (Flags | 0x1);
			invlpg(PML);
		}
		PageMapStructure* PDPT = (PageMapStructure*) (PML->Entry[PML4TIndex] & I_AlignMask);
		if(other)
			MapAddress((uint64_t) PDPT, (uint64_t) PDPT, 0x7);



		if(!(PDPT->Entry[PageDirectoryPointerTableIndex] & I_Present))
		{
			PDPT->Entry[PageDirectoryPointerTableIndex] = Memory::AllocateFromReserved() | (Flags | 0x1);
			invlpg(PML);
			invlpg(PDPT);
		}
		PageMapStructure* PageDirectory = (PageMapStructure*) (PDPT->Entry[PageDirectoryPointerTableIndex] & I_AlignMask);
		if(other)
			MapAddress((uint64_t) PageDirectory, (uint64_t) PageDirectory, 0x7);



		if(!(PageDirectory->Entry[PageDirectoryIndex] & I_Present))
		{
			PageDirectory->Entry[PageDirectoryIndex] = Memory::AllocateFromReserved() | (Flags | 0x1);
			invlpg(PML);
			invlpg(PDPT);
			invlpg(PageDirectory);
		}
		PageMapStructure* PageTable = (PageMapStructure*) (PageDirectory->Entry[PageDirectoryIndex] & I_AlignMask);
		if(other)
			MapAddress((uint64_t) PageTable, (uint64_t) PageTable, 0x7);


		PageTable->Entry[PageTableIndex] = PhysAddr | Flags;

		if(other)
		{
			UnmapAddress((uint64_t) PageTable);
			UnmapAddress((uint64_t) PageDirectory);
			UnmapAddress((uint64_t) PDPT);
			UnmapAddress((uint64_t) PML);
		}
	}





	void UnmapAddress(uint64_t VirtAddr, PageMapStructure* PML4)
	{
		bool DidMapPML4 = false;

		if(PML4 == 0)
			PML4 = GetCurrentPML4T();

		else if(PML4 != GetCurrentPML4T())
		{
			DidMapPML4 = true;
			MapAddress((uint64_t) PML4, (uint64_t) PML4, 0x03);
		}

		uint64_t PageTableIndex					= I_PT_INDEX(VirtAddr);
		uint64_t PageDirectoryIndex				= I_PD_INDEX(VirtAddr);
		uint64_t PageDirectoryPointerTableIndex	= I_PDPT_INDEX(VirtAddr);
		uint64_t PML4TIndex						= I_PML4_INDEX(VirtAddr);


		PageMapStructure* PML = (PageMapStructure*)(((PageMapStructure*) PML4)->Entry[I_RECURSIVE_SLOT] & I_AlignMask);

		if(PML)
		{
			PageMapStructure* PDPT = (PageMapStructure*)(PML->Entry[PML4TIndex] & I_AlignMask);

			if(PDPT)
			{
				PageMapStructure* PageDirectory = (PageMapStructure*)(PDPT->Entry[PageDirectoryPointerTableIndex] & I_AlignMask);

				if(PageDirectory)
				{
					PageMapStructure* PageTable = (PageMapStructure*)(PageDirectory->Entry[PageDirectoryIndex] & I_AlignMask);

					PageTable->Entry[PageTableIndex] = 0;
					invlpg((PageMapStructure*) VirtAddr);
				}
			}
		}


		if(DidMapPML4)
		{
			DidMapPML4 = false;
			if((uint64_t) PML4 != GetCurrentCR3()){ UnmapAddress((uint64_t) PML4); }
		}
	}


	void MapAddress(uint64_t VirtAddr, uint64_t PhysAddr, uint64_t Flags)
	{
		MapAddress(VirtAddr, PhysAddr, Flags, GetCurrentPML4T());
	}
	void UnmapAddress(uint64_t VirtAddr)
	{
		UnmapAddress(VirtAddr, GetCurrentPML4T());
	}


	void MapRegion(uint64_t VirtAddr, uint64_t PhysAddr, uint64_t LengthInPages, uint64_t Flags, PageMapStructure* PML4)
	{
		if(PML4 == 0)
			PML4 = GetCurrentPML4T();

		for(uint64_t i = 0; i < LengthInPages; i++)
			MapAddress(VirtAddr + (i * 0x1000), PhysAddr + (i * 0x1000), Flags, PML4);
	}

	void UnmapRegion(uint64_t VirtAddr, uint64_t LengthInPages, PageMapStructure* PML4)
	{
		if(PML4 == 0)
			PML4 = GetCurrentPML4T();

		for(uint64_t i = 0; i < LengthInPages; i++)
			UnmapAddress(VirtAddr + (i * 0x1000), PML4);
	}
}
