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

#include "export/vfs.h"

namespace vfs
{
	constexpr int TYPE_NORMAL = 0x01;
	constexpr int TYPE_FOLDER = 0x02;
	constexpr int TYPE_ROOT   = 0x80;

	// time for some fancy "modern" c++??
	struct vnode
	{
		id_t id = 0;
		int type = 0;

		std::shared_ptr<vnode> parent;
		std::vector<std::unique_ptr<vnode>> children;
	};

	struct Filesystem
	{
		nx::ipc::selector_t sel_drv;
		nx::ipc::selector_t sel_dev;

		std::string mountpoint;
		int flags = 0;
	};

	void init();


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









