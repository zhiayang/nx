// efx.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>

#include "stdint.h"
#include "krt.h"

// fuck EFI
#define STR(x) ((char16_t*) L ## x)

struct efi_guid;
struct efi_system_table;

namespace efx
{
	struct efi_allocator
	{
		static void* allocate(size_t sz);
		static void deallocate(void* pt);
		static void* reallocate(void* ptr, size_t newsz);
	};

	struct efi_aborter
	{
		static void abort(const char* fmt, ...);
		static void debuglog(const char* fmt, ...);
	};

	template<typename T>
	using array = krt::array<T, efi_allocator, efi_aborter>;

	using string = krt::string<efi_allocator, efi_aborter>;

	efx::string humanSizedBytes(size_t bytes, bool thou = false);

	void init();

	namespace options
	{
		void parse(const string& opts);
	}

	namespace fs
	{
		void discoverVolumes();
		void loadKernel();
	}


	efx::string sprint(const char* fmt, ...);
}

namespace efi
{
	void init(void* image_handle, efi_system_table* systable);

	void* image_handle();
	efi_system_table* systable();

	int print(const char* fmt, ...);
	int println(const char* fmt, ...);

	void* alloc(size_t sz);
	void free(void* ptr);


	void abort(const char* fmt, ...);
	void abort_if_error(size_t st, const char* fmt, ...);


	char16_t* convertstr(char* inp, size_t len);
	char* convertstr(char16_t* inp, size_t len);

	namespace guid
	{
		void init();

		efi_guid* fileInfo();
		efi_guid* protoUSBIO();
		efi_guid* protoDiskIO();
		efi_guid* protoBlockIO();
		efi_guid* filesystemInfo();
		efi_guid* protoDevicePath();
		efi_guid* protoLoadedImage();
		efi_guid* protoSimpleNetwork();
		efi_guid* protoDriverBinding();
		efi_guid* protoGraphicsOutput();
		efi_guid* protoManagedNetwork();
		efi_guid* protoPCIRootBrigeIO();
		efi_guid* protoSimpleFilesytem();
		efi_guid* protoSimpleTextInput();
		efi_guid* protoSimpleTextOutput();
		efi_guid* protoDevicePathToText();
		efi_guid* protoDevicePathUtilities();

		efx::string tostring(efi_guid* guid);
	}
}

namespace serial
{
	void print(char* s, size_t len);
}
















