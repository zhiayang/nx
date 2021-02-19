// internal.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <memory>
#include <optional>

#include <nx/ipc.h>
#include <nx/syscall.h>
#include <nx/rpc_message.h>

#include <zst/zpr.h>
#include <zst/result.h>

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
		off_t offset   = 0;
		uint32_t mode  = 0;
		uint16_t type  = 0;
		uint16_t flags = 0;

		// something like inode number or whatever
		uint64_t inode = 0;
	};

	constexpr uint32_t FS_FLAG_READONLY = 0x1;

	struct Filesystem
	{
		nx::ipc::selector_t sel_drv;
		nx::ipc::selector_t sel_dev;

		std::string mountpoint;
		uint32_t flags = 0;
	};

	struct ProcessState;

	struct VfsState
	{
		std::vector<vnode> vnodeTable;
		std::map<uint64_t, pid_t> connectionTable;
		std::map<pid_t, ProcessState> processTable;
		std::map<uint64_t, nx::rpc::Server> serverTable;

		std::vector<Filesystem> mountedFilesystems;
		std::map<uint64_t, std::string> registeredFilesystems;

		void* initrd;
		size_t initrdSize;
	};



	void init();
	void handleCall(uint64_t conn, nx::rpc::message_t msg);

	struct ProcessState;

	namespace fns
	{
		void rpc_registerProc(ProcessState* pst, nx::rpc::Server* srv, fns::Register msg);
		void rpc_unregisterProc(ProcessState* pst, nx::rpc::Server* srv, fns::Deregister msg);

		void rpc_bind(ProcessState* pst,  nx::rpc::Server* srv, fns::Bind msg);
		void rpc_open(ProcessState* pst,  nx::rpc::Server* srv, fns::Open msg);
		void rpc_close(ProcessState* pst, nx::rpc::Server* srv, fns::Close msg);
		void rpc_read(ProcessState* pst,  nx::rpc::Server* srv, fns::Read msg);
		void rpc_write(ProcessState* pst, nx::rpc::Server* srv, fns::Write msg);

		void rpc_spawn_fs(nx::rpc::Server* srv, fns::SpawnFilesystem msg);
		void rpc_register_fs(nx::rpc::Server* srv, fns::RegisterFilesystem msg);
		void rpc_deregister_fs(nx::rpc::Server* srv, fns::DeregisterFilesystem msg);

		void rpc_connect_initrd(nx::rpc::Server* srv, fns::ConnectInitialRamdisk msg);

		std::optional<std::tuple<void*, size_t, nx::ipc::mem_ticket_t, bool>> validate_buffer(const Buffer& buffer);
		std::optional<std::tuple<void*, size_t, nx::ipc::mem_ticket_t, bool>> validate_buffer(ProcessState* pst, const Buffer& buffer);
	}

	// utility functions
	std::vector<std::string> splitPathComponents(std::string_view path);
	std::string concatPath(const std::vector<std::string>& components);
	std::string sanitise(std::string_view path);

	bool isPathSubset(const std::vector<std::string>& total, const std::vector<std::string>& subset);
	std::vector<std::string> getFSRelativePath(Filesystem* fs, const std::vector<std::string>& components);

	Filesystem* getFilesystemForPath(std::string_view path);

	zst::Result<Handle, uint64_t> mount();
	zst::Result<Handle, uint64_t> bind(std::string_view path, Handle source, uint64_t flags);
	zst::Result<Handle, uint64_t> open(std::string_view path, uint64_t flags);
	void close(Handle handle);





	// wrappers for kernel log.
	template <typename... Args>
	void dbg(std::string_view fmt, Args&&... args)
	{
		char buf[256] = { };
		size_t len = zpr::sprint((char*) buf, 255, fmt, static_cast<Args&&>(args)...);
		syscall::kernel_log(-1, "vfs", 4, buf, len);
	}

	template <typename... Args>
	void log(std::string_view fmt, Args&&... args)
	{
		char buf[256] = { };
		size_t len = zpr::sprint((char*) buf, 255, fmt, static_cast<Args&&>(args)...);
		syscall::kernel_log(0, "vfs", 4, buf, len);
	}

	template <typename... Args>
	void warn(std::string_view fmt, Args&&... args)
	{
		char buf[256] = { };
		size_t len = zpr::sprint((char*) buf, 255, fmt, static_cast<Args&&>(args)...);
		syscall::kernel_log(1, "vfs", 4, buf, len);
	}

	template <typename... Args>
	void error(std::string_view fmt, Args&&... args)
	{
		char buf[256] = { };
		size_t len = zpr::sprint((char*) buf, 255, fmt, static_cast<Args&&>(args)...);
		syscall::kernel_log(2, "vfs", 4, buf, len);
	}
}









