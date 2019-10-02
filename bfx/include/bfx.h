// bfx.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "krt.h"

#define NX_BOOTINFO_VERSION 2
#include "../../kernel/include/bootinfo.h"


struct BOOTBOOT;
struct MMapEnt;

namespace bfx
{
	struct allocator
	{
		static void* allocate(size_t sz, size_t align);
		static void deallocate(void* pt);
	};

	struct aborter
	{
		static void abort(const char* fmt, ...);
		static void debuglog(const char* fmt, ...);
	};


	template<typename T> using array = krt::array<T, allocator, aborter>;
	template<typename T> using stack = krt::stack<T, allocator, aborter>;

	using string = krt::string<allocator, aborter>;


	void init(BOOTBOOT* bootboot);

	void abort(const char* fmt, ...);

	int print(const char* fmt, ...);
	int println(const char* fmt, ...);
	bfx::string sprint(const char* fmt, ...);

	bfx::string humanSizedBytes(size_t bytes, bool thou);



	constexpr uint64_t PAGE_SIZE = 0x1000;

	namespace idt
	{
		void init();
		void setEntry(uint8_t num, uint64_t base, uint16_t codeSegment, bool ring3Interrupt, bool nestedInterrupts);
	}

	namespace console
	{
		void init(void* fb, int x, int y, int scanx);
		void putchar(char c);
	}

	namespace exceptions
	{
		void init();
	}

	namespace initrd
	{
		krt::pair<void*, size_t> findKernel(void* ptr, size_t sz, const char* name);
	}

	namespace pmm
	{
		void init(MMapEnt* ents, size_t numEnts);

		uint64_t allocate(size_t num, bool below4G = false);
		void deallocate(uint64_t addr, size_t num);
	}

	namespace vmm
	{
		struct pml_t
		{
			uint64_t entries[512];
		};

		void bootstrap(uint64_t physBase, uint64_t virt, size_t maxPages);
		void mapAddress(uint64_t virt, uint64_t phys, size_t num, uint64_t flags);


		void setupCR3(BOOTBOOT* bbinfo);
		void mapKernel(uint64_t phys, uint64_t virt, size_t num, uint64_t flags);

		uint64_t getPML4Address();
	}

	namespace mmap
	{
		void init(MMapEnt* ents, size_t bbents);
		void addEntry(uint64_t base, size_t pages, uint64_t type);
		void finalise(nx::BootInfo* bi);
	}

	nx::BootInfo* setupBootInfo(BOOTBOOT* bbinfo);
	void loadKernel(uint8_t* input, size_t len, uint64_t* entry_out);
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






