// initrd.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "miniz.h"

namespace nx {
namespace initrd
{
	void init(BootInfo* bi)
	{
		if(bi->initrdSize == 0 || bi->initrdBuffer == 0)
			abort("no initrd!");

		// the thing is already mapped. what we need to do, is to decompress it to a buffer!
		// grab the uncompressed size. it is the last 4 bytes of the file.
		// note: the initrd is not going to be > 4gb.

		size_t inpSz = bi->initrdSize;
		void* initrd = bi->initrdBuffer;

		size_t uncompressedSize = *((uint32_t*) (((addr_t) initrd) + inpSz - sizeof(uint32_t)));
		println("initrd is %zu bytes long!", uncompressedSize);
	}
}
}