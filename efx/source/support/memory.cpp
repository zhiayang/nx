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
constexpr uintptr_t PAGE_USER    = 0x4;
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
		auto ret = efi::allocPages(1, efi::MemoryType_VMFrame, "page table");

		memset(ret, 0, 0x1000);
		return (uint64_t) ret | flags;
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
			mapVirtual(fbaddr, nx::addrs::KERNEL_FRAMEBUFFER.addr(), (graphics::getFramebufferSize() + 0xFFF) / 0x1000, 0);
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

			if(krt::match(entry->Type, EfiLoaderCode, EfiBootServicesCode, EfiBootServicesData, efi::MemoryType_BootInfo,
				efi::MemoryType_MemoryMap, efi::MemoryType_Initrd, efi::MemoryType_KernelElf))
			{
				mapVirtual(entry->PhysicalStart, entry->PhysicalStart, entry->NumberOfPages);
			}

			if(entry->Type == EfiRuntimeServicesCode || entry->Type == EfiRuntimeServicesData)
			{
				uint64_t v = nx::addrs::EFI_RUNTIME_SERVICES_BASE.addr() + entry->PhysicalStart;
				mapVirtual(entry->PhysicalStart, v, entry->NumberOfPages);
			}

			// efi::println("%2d: %p - %p   |   %x", i, entry->PhysicalStart, entry->PhysicalStart
			// 	+ (0x1000 * entry->NumberOfPages), entry->Type);
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
				entries->VirtualStart   = entry->address + nx::addrs::EFI_RUNTIME_SERVICES_BASE.addr();
				entries->NumberOfPages  = entry->numPages;
				entries->Attribute      = entry->efiAttributes;
				entries->Type           = EfiRuntimeServicesCode;
			}
			else if(entry->memoryType == nx::MemoryType::EFIRuntimeData)
			{
				entries->PhysicalStart  = entry->address;
				entries->VirtualStart   = entry->address + nx::addrs::EFI_RUNTIME_SERVICES_BASE.addr();
				entries->NumberOfPages  = entry->numPages;
				entries->Attribute      = entry->efiAttributes;
				entries->Type           = EfiRuntimeServicesData;
			}

			entries++;
		}

		auto stat = efi::systable()->RuntimeServices->SetVirtualAddressMap((uint64_t) entries - scratch, sizeof(efi_memory_descriptor),
			1, (efi_memory_descriptor*) scratch);

		bi->efi.canCallEFIRuntime = (stat == EFI_SUCCESS);
	}

	uint64_t getPML4Address()
	{
		return (uint64_t) pml4t_addr;
	}

	void installNewCR3()
	{
		asm volatile("mov %0, %%cr3" :: "r"(pml4t_addr));
	}


	void mapVirtual(uint64_t phys, uint64_t virt, size_t num, uint64_t flags)
	{
		if(!pml4t_addr) efi::abort("mapVirtual(): must call setupCR3() first!");

		constexpr uint64_t DEFAULT_FLAGS = PAGE_USER | PAGE_WRITE | PAGE_PRESENT;
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
				pml4t_addr->entries[p4idx] = allocate_pagetab(DEFAULT_FLAGS | flags);

			auto pdpt = (pml_t*) (pml4t_addr->entries[p4idx] & PAGE_ALIGN);

			if(pdpt->entries[p3idx] == 0)
				pdpt->entries[p3idx] = allocate_pagetab(DEFAULT_FLAGS | flags);

			auto pdir = (pml_t*) (pdpt->entries[p3idx] & PAGE_ALIGN);

			if(pdir->entries[p2idx] == 0)
				pdir->entries[p2idx] = allocate_pagetab(DEFAULT_FLAGS | flags);

			auto ptable = (pml_t*) (pdir->entries[p2idx] & PAGE_ALIGN);

			ptable->entries[p1idx] = p | PAGE_PRESENT | PAGE_WRITE | flags;
		}
	}


	constexpr size_t MAX_EXTENTS = 1024;
	static size_t numExtents = 0;
	static Extent customExtents[MAX_EXTENTS] = { };

	krt::pair<Extent*, size_t> getCustomExtents()
	{
		return { customExtents, numExtents };
	}

	static void accountMemory(uint64_t base, size_t num, uint32_t type)
	{
		auto& ext = customExtents[numExtents++];
		if(numExtents >= MAX_EXTENTS)
			efi::abort("out of space for custom extents! (exceeded %zu)", MAX_EXTENTS);

		// translate from the efi type to the type we want in the kernel memory map!!
		switch(type)
		{
			case efi::MemoryType_VMFrame:       // fallthrough
			case efi::MemoryType_LoadedKernel:  type = (uint32_t) nx::MemoryType::LoadedKernel; break;
			case efi::MemoryType_MemoryMap:     type = (uint32_t) nx::MemoryType::MemoryMap;    break;
			case efi::MemoryType_BootInfo:      type = (uint32_t) nx::MemoryType::BootInfo;     break;
			case efi::MemoryType_Initrd:        type = (uint32_t) nx::MemoryType::Initrd;       break;
			case efi::MemoryType_KernelElf:     type = (uint32_t) nx::MemoryType::KernelElf;    break;
		}

		ext.base = base;
		ext.type = type;
		ext.num  = num;
		ext.flag = 0;
	}
}
}

namespace efi
{
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


	void* allocPages(size_t numPages, uint32_t memType, const char* user)
	{
		auto st = efi::systable();

		//! note: apparently many firmwares (including virtualbox 6.1.x, and some recorded instances of real hardware)
		//!       have a bug where they don't handle custom memory types properly, causing page faults.
		//* (usually because they use the type as an index into an array of size MaxMemoryType uwu)

		// so, to give the kernel the same level of information (specifically, being able to discriminate between the initrd,
		// loadedelf, etc.), we must implement our own book-keeping.

		// this is LoaderCode instead of LoaderData because there's a bunch of implicit assumptions throughout efx that
		// LoaderData is unimportant and unmappable, so we will fix it as loadercode.
		auto efiType = memType;
		if(efiType > EfiMaxMemoryType)
			efiType = EfiLoaderCode;

		uint64_t out = 0;
		auto stat = st->BootServices->AllocatePages(AllocateAnyPages, (efi_memory_type) efiType, numPages, &out);
		efi::abort_if_error(stat, "failed to allocate memory for %s!", user);

		// we need to pass the originally intended type to the accounting part.
		efx::memory::accountMemory(out, numPages, memType);

		return (void*) out;
	}
}












