// initrd.cpp
// Copyright (c) 2021, zhiayang
// Licensed under the Apache License Version 2.0.

#include <nx/rpc.h>
#include <nx/ipc.h>
#include <sys/mman.h>


#include "internal.h"
#include "export/vfs.h"

namespace vfs
{
	extern VfsState state;

	void fns::rpc_connect_initrd(nx::rpc::Server* srv, ConnectInitialRamdisk msg)
	{
		auto ubuf = validate_buffer(msg.initrd);
		if(!ubuf)
		{
			srv->error<ConnectInitialRamdisk>(ERR_INVALID_BUFFER);
			return;
		}

		auto [ ptr, len, tik, rel ] = *ubuf;
		void* buffer = mmap(nullptr, len, PROT_READ | PROT_WRITE, MAP_ANON, 0, 0);
		if(buffer == (void*) -1)
			error("failed to mmap ({} bytes requested)", len);

		memcpy(buffer, ptr, len);
		if(rel) nx::ipc::release_memory_ticket(tik);

		state.initrd = buffer;
		state.initrdSize = len;

		log("vfs initialised with {} kb of initrd (ptr: {#x})", state.initrdSize / 1024, state.initrd);
	}
}
