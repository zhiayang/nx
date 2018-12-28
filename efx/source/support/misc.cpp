// misc.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "efx.h"
#include "efi/system-table.h"

volatile void* __dso_handle;

namespace efi
{
	static efi_handle img_handle = 0;
	static efi_system_table* system_table = 0;

	void init(void* handle, efi_system_table* st)
	{
		system_table = st;
		img_handle = handle;
	}

	void* image_handle()
	{
		return img_handle;
	}

	efi_system_table* systable()
	{
		return system_table;
	}
}

bool operator == (const efi_guid& a, const efi_guid& b)
{
	return memcmp(&a.data[0], &b.data[0], 16) == 0;
}