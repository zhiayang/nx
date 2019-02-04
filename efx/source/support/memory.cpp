// memory.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "efx.h"
#include "efi/system-table.h"
#include "efi/boot-services.h"

#include "string.h"

// for the vmm structs.
#include "../../kernel/include/misc/addrs.h"


constexpr uintptr_t PAGE_PRESENT = 0x1;
constexpr uintptr_t PAGE_WRITE   = 0x2;
constexpr uintptr_t PAGE_ALIGN   = ~0xFFF;

struct pml_t
{
	uint64_t entries[512];
};

constexpr size_t indexPML4(uintptr_t addr)       { return ((((uintptr_t) addr) >> 39) & 0x1FF); }
constexpr size_t indexPDPT(uintptr_t addr)       { return ((((uintptr_t) addr) >> 30) & 0x1FF); }
constexpr size_t indexPageDir(uintptr_t addr)    { return ((((uintptr_t) addr) >> 21) & 0x1FF); }
constexpr size_t indexPageTable(uintptr_t addr)  { return ((((uintptr_t) addr) >> 12) & 0x1FF); }


namespace efx
{
	void* efi_allocator::allocate(size_t sz, size_t align)  { return efi::alloc(sz); }
	void efi_allocator::deallocate(void* ptr)               { return efi::free(ptr); }
}

namespace efx {
namespace memory
{
	static uint64_t allocate_pagetab(uint64_t flags)
	{
		uint64_t ret = 0;

		auto bs = efi::systable()->BootServices;
		auto stat = bs->AllocatePages(AllocateAnyPages, (efi_memory_type) efi::MemoryType_VMFrame, 1, &ret);
		efi::abort_if_error(stat, "failed to allocate page!");

		memset((void*) ret, 0, 0x1000);
		return ret | flags;
	}

	static pml_t* pml4t_addr = 0;
	void setupCR3()
	{
		efi::println("setting up kernel CR3");

		// pml4:    256 tb
		// pdpt:    512 gb
		// pd:      1 gb
		// pt:      2 mb
		// pe:      4 kb

		// identity map the first 4 mb -- requiring 2 page-table entries.
		auto pml4 = (pml_t*) allocate_pagetab(0);
		auto pdpt = (pml_t*) ((pml4->entries[0] = allocate_pagetab(PAGE_PRESENT | PAGE_WRITE)) & PAGE_ALIGN);
		auto pagedir = (pml_t*) ((pdpt->entries[0] = allocate_pagetab(PAGE_PRESENT | PAGE_WRITE)) & PAGE_ALIGN);

		// we need 2 page tables.
		auto pt1 = (pml_t*) ((pagedir->entries[0] = allocate_pagetab(PAGE_PRESENT | PAGE_WRITE)) & PAGE_ALIGN);
		auto pt2 = (pml_t*) ((pagedir->entries[1] = allocate_pagetab(PAGE_PRESENT | PAGE_WRITE)) & PAGE_ALIGN);

		// identity map them.
		for(uint64_t i = 0; i < 512; i++) pt1->entries[i] = (i * 0x1000) | PAGE_PRESENT | PAGE_WRITE;
		for(uint64_t i = 0; i < 512; i++) pt2->entries[i] = ((i + 512) * 0x1000) | PAGE_PRESENT | PAGE_WRITE;

		// also, recursively map.
		// BUT! since we are using the top 2GB for our higher-half kernel, we do the recursive map in the second-last
		// slot (index 510) instead of the last one (511).
		pml4->entries[510] = ((uint64_t) pml4 | PAGE_PRESENT | PAGE_WRITE);

		pml4t_addr = pml4;

		// ok, map the framebuffer as well.
		if(auto fbaddr = graphics::getFramebufferAddress(); fbaddr != 0)
		{
			mapVirtual(fbaddr, nx::addrs::KERNEL_FRAMEBUFFER, (graphics::getFramebufferSize() + 0x1000) / 0x1000);
		}
	}

	void finaliseMappingExistingMemory()
	{
		auto bs = efi::systable()->BootServices;

		size_t key = 0;
		size_t bufSz = 1;
		size_t descSz = 0;
		uint32_t descVer = 0;

		size_t numEnts = 0;
		uint8_t* buffer = 0;
		{
			uint8_t buf = 0;

			// this is a really poorly designed function interface.
			auto stat = bs->GetMemoryMap(&bufSz, (efi_memory_descriptor*) &buf, &key, &descSz, &descVer);
			if(stat != EFI_BUFFER_TOO_SMALL) efi::abort_if_error(stat, "setupCR3(): failed to get memory map (1)");
		}
		{
			buffer = (uint8_t*) efi::alloc(bufSz + 256);
			bufSz += 256;

			auto stat = bs->GetMemoryMap(&bufSz, (efi_memory_descriptor*) buffer, &key, &descSz, &descVer);
			efi::abort_if_error(stat, "setupCR3(): failed to get memory map (2)");

			numEnts = bufSz / descSz;
		}

		for(size_t i = 0; i < numEnts; i++)
		{
			auto entry = (efi_memory_descriptor*) (buffer + (i * descSz));

			if(krt::match(entry->Type, EfiLoaderCode, EfiBootServicesCode, EfiBootServicesData, efi::MemoryType_BootInfo, efi::MemoryType_MemoryMap, efi::MemoryType_Initrd, efi::MemoryType_KernelElf))
			{
				mapVirtual(entry->PhysicalStart, entry->PhysicalStart, entry->NumberOfPages);
			}

			if(entry->Type == EfiRuntimeServicesCode || entry->Type == EfiRuntimeServicesData)
			{
				uint64_t v = nx::addrs::EFI_RUNTIME_SERVICES_BASE + entry->PhysicalStart;
				mapVirtual(entry->PhysicalStart, v, entry->NumberOfPages);
			}
		}
	}

	void setEFIMemoryMap(nx::BootInfo* bi, uint64_t scratch)
	{
		// what we need to do is grab all the entries that are marked as EfiRuntimeServices,
		// and mark them with the new base address. (which is just offset to EFI_RUNTIME_SERVICES_BASE)

		// ok, we got 4k of scratch space for this new shitty memory map.
		// but we only need to care about the stuff that's tagged as runtime services.


		auto entries = (efi_memory_descriptor*) scratch;
		for(size_t i = 0; i < bi->mmEntryCount; i++)
		{
			auto entry = &bi->mmEntries[i];
			if(entry->memoryType == nx::MemoryType::EFIRuntimeCode)
			{
				entries->PhysicalStart  = entry->address;
				entries->VirtualStart   = entry->address + nx::addrs::EFI_RUNTIME_SERVICES_BASE;
				entries->NumberOfPages  = entry->numPages;
				entries->Attribute      = entry->efiAttributes;
				entries->Type           = EfiRuntimeServicesCode;
			}
			else if(entry->memoryType == nx::MemoryType::EFIRuntimeData)
			{
				entries->PhysicalStart  = entry->address;
				entries->VirtualStart   = entry->address + nx::addrs::EFI_RUNTIME_SERVICES_BASE;
				entries->NumberOfPages  = entry->numPages;
				entries->Attribute      = entry->efiAttributes;
				entries->Type           = EfiRuntimeServicesData;
			}

			entries++;
		}

		auto stat = efi::systable()->RuntimeServices->SetVirtualAddressMap((uint64_t) entries - scratch, sizeof(efi_memory_descriptor),
			1, (efi_memory_descriptor*) scratch);

		bi->canCallEFIRuntime = (stat == EFI_SUCCESS);
	}

	uint64_t getPML4Address()
	{
		return (uint64_t) pml4t_addr;
	}

	void installNewCR3()
	{
		asm volatile("mov %0, %%cr3" :: "r"(pml4t_addr));
	}


	void mapVirtual(uint64_t phys, uint64_t virt, size_t num)
	{
		if(!pml4t_addr) efi::abort("mapVirtual(): must call setupCR3() first!");

		for(size_t i = 0; i < num; i++)
		{
			uint64_t p = phys + (0x1000 * i);
			uint64_t v = virt + (0x1000 * i);

			// right.
			auto p4idx = indexPML4(v);
			auto p3idx = indexPDPT(v);
			auto p2idx = indexPageDir(v);
			auto p1idx = indexPageTable(v);

			if(p4idx == 510) efi::abort("mapVirtual(): cannot map to 510th PML4 entry!");

			if(pml4t_addr->entries[p4idx] == 0)
				pml4t_addr->entries[p4idx] = allocate_pagetab(PAGE_PRESENT | PAGE_WRITE);

			auto pdpt = (pml_t*) (pml4t_addr->entries[p4idx] & PAGE_ALIGN);


			if(pdpt->entries[p3idx] == 0)
				pdpt->entries[p3idx] = allocate_pagetab(PAGE_PRESENT | PAGE_WRITE);

			auto pdir = (pml_t*) (pdpt->entries[p3idx] & PAGE_ALIGN);


			if(pdir->entries[p2idx] == 0)
				pdir->entries[p2idx] = allocate_pagetab(PAGE_PRESENT | PAGE_WRITE);

			auto ptable = (pml_t*) (pdir->entries[p2idx] & PAGE_ALIGN);

			ptable->entries[p1idx] = p | PAGE_PRESENT | PAGE_WRITE;
		}
	}
}
}






















namespace efi
{
	#define BLKHEADER_MAGIC 0xDEADC0DEBEEFFACE

	// add some rudimentary stuff
	struct blkheader_t
	{
		uint64_t magic;
		size_t length;
	};

	void* alloc(size_t sz)
	{
		if(sz == 0) return nullptr;

		void* ret = 0;
		auto stat = efi::systable()->BootServices->AllocatePool(EfiLoaderData, sz, &ret);
		if(EFI_ERROR(stat) || ret == 0)
			efi::abort("failed to allocate memory! (requested %zu bytes) (status: %d)", sz, stat);

		memset(ret, 0, sz);
		return ret;
	}

	void free(void* ptr)
	{
		if(ptr) efi::systable()->BootServices->FreePool(ptr);
	}
}
