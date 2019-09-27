// bfx.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "krt.h"

#define NX_BOOTINFO_VERSION 3
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



	constexpr uint64_t PAGE_SIZE        = 0x1000;

	namespace idt
	{
		void init();
		void setEntry(uint8_t num, uint64_t base, uint16_t codeSegment, bool ring3Interrupt, bool nestedInterrupts);
	}

	namespace exceptions
	{
		void init();
	}

	namespace pmm
	{
		void init(MMapEnt* ents, size_t numEnts);

		uint64_t allocate(size_t num, bool below4G = false);
		void deallocate(uint64_t addr, size_t num);
	}

	namespace vmm
	{
		void bootstrap(uint64_t physBase, uint64_t virt, size_t maxPages);

		void mapAddress(uint64_t virt, uint64_t phys, size_t num, uint64_t flags);
		void unmapAddress(uint64_t virt, size_t num, bool freePhys);
		uint64_t getPhysAddr(uint64_t virt);
		bool isMapped(uint64_t virt, size_t num);

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






