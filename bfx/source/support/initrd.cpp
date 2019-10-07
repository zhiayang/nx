// initrd.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "bfx.h"
#include "string.h"

namespace bfx {
namespace initrd
{
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

		// ustar format
		char type;
		char link_name[100];
		char ustar[8];
		char owner[32];
		char group[32];
		char major[8];
		char minor[8];
		char name_prefix[155];
	};

	static size_t octalToNumber(char oct[12])
	{
		size_t ret = 0;
		int l = 12;

		size_t mul = 1;
		for(int i = l - 1; i-- > 0;)
		{
			ret += mul * (oct[i] - '0');
			mul *= 8;
		}

		return ret;
	}


	static uint8_t* initrdPtr = 0;
	static size_t initrdSize = 0;

	krt::pair<void*, size_t> findFile(void* ptr, size_t sz, const char* kernel_name)
	{
		// note: bootboot always decompresses the initrd for us, so it will always
		// come out as a tar file. no need to bother with un-gzipping this.

		initrdPtr   = (uint8_t*) ptr;
		initrdSize  = sz;

		if(memcmp(((tarent_t*) initrdPtr)->ustar, "ustar\0""00", 8) != 0)
			abort("invalid (non-tar) initrd format!");

		for(size_t i = 0; i < initrdSize;)
		{
			if(initrdPtr[i] == 0)
			{
				i += 512;
				continue;
			}

			auto ent = (tarent_t*) (initrdPtr + i);

			// just make a stack buffer.
			char fullname[256] = { 0 };
			{
				char* ptr = fullname;
				auto len1 = strlen(ent->name_prefix);

				strncpy(ptr, ent->name_prefix, len1);
				ptr += len1;

				auto len2 = strlen(ent->name);
				strncpy(ptr, ent->name, len2);

				fullname[len1+len2] = 0;
			}

			if(strcmp(fullname, kernel_name) == 0)
			{
				return {
					(void*) (initrdPtr + (i + 512)),
					octalToNumber(ent->size)
				};
			}

			auto filesize = octalToNumber(ent->size);
			auto s = ((filesize + 511) & ~511);
			i += __max((size_t) 512, ((512 + s) / 512) * 512);
		}

		return { 0, 0 };
	}
}
}














