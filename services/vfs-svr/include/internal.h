// internal.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include <vector>
#include <string>
#include <memory>

#include <nx/ipc.h>
#include <nx/rpc_message.h>

#include "export/vfs.h"

namespace nx::rpc
{
	struct Server;
}

namespace vfs
{
	constexpr uint16_t TYPE_NORMAL = 0x01;
	constexpr uint16_t TYPE_FOLDER = 0x02;
	constexpr uint16_t TYPE_ROOT   = 0x80;

	// time for some fancy "modern" c++??
	struct vnode
	{
		id_t id = 0;

		off_t offset   = 0;
		uint32_t mode  = 0;
		uint16_t type  = 0;
		uint16_t flags = 0;

		// something like inode number or whatever
		uint64_t driverData = 0;

		std::shared_ptr<vnode> parent;
		std::vector<std::shared_ptr<vnode>> children;
	};

	struct Filesystem
	{
		nx::ipc::selector_t sel_drv;
		nx::ipc::selector_t sel_dev;

		std::string mountpoint;
		uint32_t flags = 0;
	};

	void init();
	void handleCall(uint64_t conn, nx::rpc::message_t msg);

	struct ProcessState;

	namespace fns
	{
		void registerProc(ProcessState* pst, nx::rpc::Server* srv, fns::FnRegister msg);
		void unregisterProc(ProcessState* pst, nx::rpc::Server* srv, fns::FnDeregister msg);

		void bind(ProcessState* pst,  nx::rpc::Server* srv, fns::FnBind msg);
		void open(ProcessState* pst,  nx::rpc::Server* srv, fns::FnOpen msg);
		void close(ProcessState* pst, nx::rpc::Server* srv, fns::FnClose msg);
		void read(ProcessState* pst,  nx::rpc::Server* srv, fns::FnRead msg);
		void write(ProcessState* pst, nx::rpc::Server* srv, fns::FnWrite msg);
	}

	// utility functions
	std::vector<std::string> splitPathComponents(const std::string& path);
	std::string concatPath(const std::vector<std::string>& components);
	std::string sanitise(const std::string& path);

	bool isPathSubset(const std::vector<std::string>& total, const std::vector<std::string>& subset);
	std::vector<std::string> getFSRelativePath(Filesystem* fs, const std::vector<std::string>& components);


	// wrappers for kernel log.
	void dbg(const char* fmt, ...);
	void log(const char* fmt, ...);
	void warn(const char* fmt, ...);
	void error(const char* fmt, ...);
}









