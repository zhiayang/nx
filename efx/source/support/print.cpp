// print.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdarg.h>

#include "krt.h"
#include "efx.h"

#include "string.h"
#include "efi/system-table.h"


namespace efi
{
	static size_t cb_efiprint(void* ctx, const char* s, size_t len)
	{
		char16_t* s16 = convertstr((char*) s, len);
		efi::systable()->ConOut->OutputString(efi::systable()->ConOut, s16);

		// mirror to the serial port as well.
		serial::print((char*) s, len);

		return len;
	}

	static int vprint(const char* fmt, va_list args)
	{
		return vcbprintf(nullptr, cb_efiprint, fmt, args);
	}

	static void vabort(const char* fmt, va_list args)
	{
		print("error: ");
		vprint(fmt, args);
		print("\nan error occured while loading the kernel; returning control to the bootloader\n\n");
	}



	void abort(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);
		vabort(fmt, args);
		va_end(args);

		efi::systable()->BootServices->Exit(efi::image_handle(), EFI_UNSUPPORTED, 0, nullptr);
	}

	void abort_if_error(size_t st, const char* fmt, ...)
	{
		if(!EFI_ERROR(st)) return;

		va_list args; va_start(args, fmt);
		vabort(fmt, args);
		println("error code: %d", st);
		va_end(args);

		efi::systable()->BootServices->Exit(efi::image_handle(), EFI_UNSUPPORTED, 0, nullptr);
	}

	int print(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);

		int ret = vprint(fmt, args);

		va_end(args);
		return ret;
	}

	int println(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);

		int ret = vprint(fmt, args);
		ret += cb_efiprint(nullptr, "\n", 1);

		va_end(args);
		return ret;
	}




	constexpr size_t BUFFER_LEN = 512;
	static char c8_buffer[BUFFER_LEN];
	static char16_t c16_buffer[BUFFER_LEN];

	char16_t* convertstr(char* inp, size_t len)
	{
		size_t i = 0;
		for(; i < __min(len, BUFFER_LEN) && *inp; i++, inp++)
		{
			if(*inp == '\n')
			{
				// add \r for them
				c16_buffer[i] = '\r';
				c16_buffer[++i] = '\n';
			}
			else
			{
				c16_buffer[i] = *inp;
			}
		}

		c16_buffer[i] = 0;
		return &c16_buffer[0];
	}

	char* convertstr(char16_t* inp, size_t len)
	{
		size_t i = 0;
		for(; i < __min(len, BUFFER_LEN) && *inp; i++, inp++)
		{
			if(*inp == '\r')
			{
				// skip carriage return
			}
			else
			{
				c8_buffer[i] = (char) *inp;
			}
		}

		c8_buffer[i] = 0;
		return &c8_buffer[0];
	}
}



namespace efx
{
	void efi_aborter::abort(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);
		efi::vabort(fmt, args);

		va_end(args);
		efi::systable()->BootServices->Exit(efi::image_handle(), EFI_UNSUPPORTED, 0, nullptr);
	}

	void efi_aborter::debuglog(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);

		efi::print("debug: ");
		efi::vprint(fmt, args);
		efi::print("\n");

		va_end(args);
	}

}

























