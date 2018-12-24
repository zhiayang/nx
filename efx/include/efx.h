// efx.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <stddef.h>

// fuck EFI
#define STR(x) ((char16_t*) L ## x)

struct efi_system_table;

namespace efi
{
	void init_systable(efi_system_table* systable);
	efi_system_table* systable();

	int printf(const char* fmt, ...);






	char16_t* convertstr(const char* inp, size_t len);
}