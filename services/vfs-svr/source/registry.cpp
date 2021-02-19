// registry.cpp
// Copyright (c) 2021, zhiayang
// Licensed under the Apache License Version 2.0.

#include <nx/rpc.h>

#include "internal.h"
#include "export/vfs.h"

using namespace nx;

namespace vfs
{
	extern VfsState state;
}

namespace vfs::fns
{
	void rpc_register_fs(rpc::Server* srv, RegisterFilesystem msg)
	{
		auto _buf = validate_buffer(msg.serverPath);
		if(!_buf)
		{
			srv->error<RegisterFilesystem>(ERR_INVALID_BUFFER);
			return;
		}

		std::string serverPath;
		{
			// copy the path and release the buffer asap.
			auto [ buf, len, memticket, needs_release ] = *_buf;
			serverPath = sanitise(std::string((const char*) buf, len));
			if(needs_release)
				ipc::release_memory_ticket(memticket);
		}

		if(auto it = state.registeredFilesystems.find(msg.fs_type); it != state.registeredFilesystems.end())
		{
			error("fs_type '{}' already registered (existing path: {}, new path: {})",
				msg.fs_type, it->second, serverPath);
			srv->error<RegisterFilesystem>(ERR_FS_CODE_EXISTS);
		}

		state.registeredFilesystems[msg.fs_type] = serverPath;
		log("registered fs server '{}' with type '{}'", serverPath, msg.fs_type);

		srv->reply<RegisterFilesystem>();
	}

	void rpc_deregister_fs(rpc::Server* srv, DeregisterFilesystem msg)
	{
		srv->error<DeregisterFilesystem>(ERR_NOT_IMPLEMENTED);
	}
}
