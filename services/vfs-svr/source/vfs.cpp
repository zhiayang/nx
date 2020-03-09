// vfs.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "internal.h"
#include "export/vfs.h"

using selector_t = nx::ipc::selector_t;

namespace vfs
{
	static struct {

		std::unique_ptr<vnode> root;
		std::vector<std::unique_ptr<Filesystem>> filesystems;

	} state;

	void init()
	{
		state.root = std::make_unique<vnode>();
		state.root->type = TYPE_ROOT;

		// log("root initialised");
	}

	int mount(const char* mountpt, selector_t sel_drv, selector_t sel_dev, int flags)
	{
		auto fs = std::make_unique<Filesystem>();

		fs->flags       = flags;
		fs->mountpoint  = sanitise(mountpt);

		fs->sel_drv = sel_drv;
		fs->sel_dev = sel_dev;

		state.filesystems.push_back(std::move(fs));

		return 0;
	}
}

















