// initrd.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "tinf.h"

#define NX_BOOTINFO_VERSION NX_SUPPORTED_BOOTINFO_VERSION
#include "bootinfo.h"


extern "C" void printline(const char* s, ...)
{
	va_list args; va_start(args, s);
	nx::vprint(s, args);
	va_end(args);
}

struct gzip_header_t
{
	uint8_t magic[2];
	uint8_t compression;
	uint8_t flags;
	uint32_t timestamp;

	uint8_t extra;
	uint8_t os;

} __attribute__((packed));

namespace nx {
namespace initrd
{
	void init(BootInfo* bi)
	{
		tinf_init();

		if(bi->initrdSize == 0 || bi->initrdBuffer == 0)
			abort("no initrd!");

		// the thing is already mapped. what we need to do, is to decompress it to a buffer!
		// grab the uncompressed size. it is the last 4 bytes of the file.
		// note: the initrd is not going to be > 4gb.

		size_t inpSz = bi->initrdSize;
		void* initrd = bi->initrdBuffer;

		size_t uncompressedSize = 0;

		// note: we do this to avoid unaligned access, which ubsan complains about.
		{
			uint32_t tmp = 0;
			memcpy(&tmp, (uint32_t*) (((addr_t) initrd) + inpSz - sizeof(uint32_t)), sizeof(uint32_t));

			uncompressedSize = tmp;
		}

		// make a thing.
		void* buf = (void*) vmm::allocate((uncompressedSize + PAGE_SIZE) / PAGE_SIZE, vmm::AddressSpace::Kernel);
		{
			auto hdr = (gzip_header_t*) initrd;
			assert(hdr->magic[0] == 0x1F);
			assert(hdr->magic[1] == 0x8B);

			if(hdr->compression != 8) abort("initrd in unsupported format!");

			uint8_t* data = (uint8_t*) (hdr + 1);
			if(hdr->flags & 0x4)
				data += *((uint16_t*) data);

			if(hdr->flags & 0x8)    { while(*data) data++; data++; }
			if(hdr->flags & 0x10)   { while(*data) data++; data++; }
			if(hdr->flags & 0x20)   { abort("initrd is encrypted?!"); }

			unsigned int len = (int) uncompressedSize;
			int ret = tinf_uncompress(buf, &len, data, (int) inpSz);
			if(ret != TINF_OK || len != (unsigned int) uncompressedSize)
				abort("failed to decompress initrd!");
		}

		log("initrd", "decompressed initrd: %s", humanSizedBytes(uncompressedSize).cstr());

		auto tarfs = vfs::tarfs::create((uint8_t*) buf, uncompressedSize);
		vfs::mount(tarfs, "/initrd", true);


		// free the old buffer.
		pmm::freeEarlyMemory(bi, MemoryType::Initrd);
	}
}
}















