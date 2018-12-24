// misc.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdarg.h>

#include "krt.h"
#include "efx.h"

#include "string.h"
#include "efi/system-table.h"


namespace efi
{
	static efi_system_table* system_table = 0;
	void init_systable(efi_system_table* st)
	{
		system_table = st;
	}

	efi_system_table* systable()
	{
		return system_table;
	}



	static size_t cb_efiprint(void* ctx, const char* s, size_t len)
	{
		char16_t* s16 = convertstr(s, len);
		system_table->ConOut->OutputString(system_table->ConOut, s16);

		return len;
	}

	int printf(const char* fmt, ...)
	{
		// cb_efiprint(nullptr, fmt, strlen(fmt));
		va_list args;
		va_start(args, fmt);

		int ret = vcbprintf(nullptr, cb_efiprint, fmt, args);
		va_end(args);

		return ret;

		// return 0;
	}

	constexpr size_t BUFFER_LEN = 256;
	static char16_t buffer[BUFFER_LEN];

	char16_t* convertstr(const char* inp, size_t len)
	{
		size_t i = 0;
		for(; i < __min(len, BUFFER_LEN); i++, inp++)
		{
			if(*inp == '\n')
			{
				// add \r for them
				buffer[i] = '\r';
				buffer[++i] = '\n';
			}
			else
			{
				buffer[i] = *inp;
			}
		}

		buffer[i] = 0;
		return &buffer[0];
	}
}



























