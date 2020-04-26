// initrd.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "tinf.h"

#define NX_BOOTINFO_VERSION NX_MAX_BOOTINFO_VERSION
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

struct tarent_t
{
	// original format
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char check[8];
	char typr;
	char link_name[100];

	// ustar format
	char ustar[8];
	char owner[32];
	char group[32];
	char major[8];
	char minor[8];
	char name_prefix[155];

// should already be packed anyway.
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

		void* finalPtr = 0;
		size_t finalSz = 0;


		// check if it's gzip first.
		if(((gzip_header_t*) initrd)->magic[0] == 0x1F && ((gzip_header_t*) initrd)->magic[1] == 0x8B)
		{
			log("initrd", "format: gzip");
			println("decompressing initrd...\n");

			// note: we do this to avoid unaligned access, which ubsan complains about.
			size_t uncompressedSize = 0;
			{
				uint32_t tmp = 0;
				memcpy(&tmp, (void*) (((addr_t) initrd) + inpSz - sizeof(uint32_t)), sizeof(uint32_t));

				uncompressedSize = tmp;
			}

			// make a thing.
			void* buf = (void*) vmm::allocateEager((uncompressedSize + PAGE_SIZE - 1) / PAGE_SIZE,
				vmm::AddressSpaceType::Kernel, vmm::PAGE_WRITE).addr();

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

			finalPtr = buf;
			finalSz = uncompressedSize;
		}
		else if(memcmp(((tarent_t*) initrd)->ustar, "ustar\0""00", 8) == 0)
		{
			log("initrd", "format: tar");

			void* buf = (void*) vmm::allocateEager((inpSz + PAGE_SIZE - 1) / PAGE_SIZE,
				vmm::AddressSpaceType::Kernel, vmm::PAGE_WRITE).addr();

			memcpy(buf, initrd, inpSz);

			finalPtr = buf;
			finalSz = inpSz;
		}
		else
		{
			error("initrd", "unsupported format");
			abort("failed to load initrd!");
		}

		log("initrd", "size: %zu kb", finalSz / 1024);

		auto tarfs = vfs::tarfs::create((uint8_t*) finalPtr, finalSz);
		if(!vfs::mount(tarfs, "/initrd", true))
			abort("failed to mount initrd!");

		// free the old buffer.
		pmm::freeEarlyMemory(bi, MemoryType::Initrd);
	}
}
}















