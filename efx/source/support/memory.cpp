// memory.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "efx.h"
#include "efi/system-table.h"
#include "efi/boot-services.h"

#include "string.h"

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

namespace efx
{
	void* efi_allocator::allocate(size_t sz)                { return efi::alloc(sz); }
	void efi_allocator::deallocate(void* ptr)               { return efi::free(ptr); }
}





















