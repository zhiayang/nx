// program.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "loader.h"

#include <elf.h>

namespace nx {
namespace loader
{
	static nx::string getProcName(const nx::string& path)
	{
		for(size_t i = path.size(); i-- > 0; )
		{
			if(path[i] == '/')
				return path.substring(i + 1);
		}

		return path;
	}

	scheduler::Thread* loadProgram(const nx::string& path)
	{
		auto f = vfs::open(path, vfs::Mode::Read);
		assert(f);

		auto sz = vfs::stat(f).fileSize;
		auto buf = new uint8_t[sz];

		auto didRead = vfs::read(f, buf, sz);
		if(didRead != sz) abort("size mismatch! %zu / %zu", didRead, sz);

		auto proc = scheduler::createProcess(getProcName(path), scheduler::Process::PROC_USER);

		addr_t entryPt = 0;
		bool success = loadIndeterminateBinary(proc, buf, sz, &entryPt);
		if(success)
		{
			auto thr = scheduler::createThread(proc, (scheduler::Fn0Args_t) entryPt);
			return thr;
		}
		else
		{
			error("loader", "failed to load program %s!", path.cstr());
			return 0;
		}
	}






	// note: we put it here cos for now elfs (elves?) are the only thing we support
	bool loadIndeterminateBinary(scheduler::Process* proc, void* buf, size_t len, addr_t* entry)
	{
		assert(buf);
		assert(proc);
		assert(len > 0);

		// check for ELF
		{
			auto tmp = (char*) buf;
			if(tmp[0] == ELFMAG0 && tmp[1] == ELFMAG1 && tmp[2] == ELFMAG2 && tmp[3] == ELFMAG3)
				return loadELFBinary(proc, buf, len, entry);
		}

		// oopss.
		error("loader", "unable to determine executable format; loading failed");
		return false;
	}


}
}


















