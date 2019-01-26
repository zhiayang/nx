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

bool operator == (const efi_guid& a, const efi_guid& b);

namespace nx
{
	struct BootInfo;
}

namespace efx
{
	struct efi_allocator
	{
		static void* allocate(size_t sz, size_t align);
		static void deallocate(void* pt);
	};

	struct efi_aborter
	{
		static void abort(const char* fmt, ...);
		static void debuglog(const char* fmt, ...);
	};

	template<typename T> using array = krt::array<T, efi_allocator, efi_aborter>;
	template<typename T> using stack = krt::stack<T, efi_allocator, efi_aborter>;

	using string = krt::string<efi_allocator, efi_aborter>;

	efx::string humanSizedBytes(size_t bytes, bool thou = false);

	void init();

	namespace options
	{
		void parse(const string& opts);

		bool has_option(const string& opt);
		string get_option(const string& opt);
	}

	namespace fs
	{
		void discoverVolumes(void* currentHandle);
		void setRootFilesystemFromOpts();

		string readFile(const efx::string& path);
		uint8_t* readFile(const efx::string& path, size_t* size);
	}

	namespace graphics
	{
		void setDisplayMode(int defaultX, int defaultY);
		bool isGraphical();

		int getX();
		int getY();
		int getPixelsPerScanline();

		uint64_t getFramebufferAddress();
		size_t getFramebufferSize();
	}

	namespace memory
	{
		void setupCR3();
		void mapVirtual(uint64_t phys, uint64_t virt, size_t num);

		uint64_t getPML4Address();

		void finaliseMappingExistingMemory();

		void installNewCR3();
		void setEFIMemoryMap(nx::BootInfo* bi, uint64_t scratch);
	}

	void loadKernel(uint8_t* buf, size_t len, uint64_t* entry);
	nx::BootInfo* prepareKernelBootInfo();
	void setKernelMemoryMap(nx::BootInfo* bootinfo);
	void exitBootServices();

	efx::string sprint(const char* fmt, ...);
}

namespace efi
{
	static constexpr int MemoryType_LoadedKernel    = 0x80000001;
	static constexpr int MemoryType_BootInfo        = 0x80000002;
	static constexpr int MemoryType_MemoryMap       = 0x80000003;
	static constexpr int MemoryType_VMFrame         = 0x80000004;
	static constexpr int MemoryType_Initrd          = 0x80000005;
	static constexpr int MemoryType_KernelElf       = 0x80000006;


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

		efx::string tostring(const efi_guid& guid);
		efi_guid parse(const efx::string& str);
	}
}

namespace serial
{
	void print(char* s, size_t len);
}



#ifdef FUCKIN_WSL_FIX_YOUR_SHIT
[[nodiscard]] void* operator new    (unsigned long long count, void* ptr);
[[nodiscard]] void* operator new[]  (unsigned long long count, void* ptr);
#else
[[nodiscard]] void* operator new    (size_t count, void* ptr);
[[nodiscard]] void* operator new[]  (size_t count, void* ptr);
#endif












