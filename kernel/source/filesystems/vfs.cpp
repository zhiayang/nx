// vfs.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace vfs
{
	static nx::array<Node*> OpenNodes;
	static nx::array<Filesystem*> MountedFilesystems;

	static id_t NodeIdCounter;
	static id_t FilesystemIdCounter;

	void init()
	{
		OpenNodes = nx::array<Node*>();
		MountedFilesystems = nx::array<Filesystem*>();
	}

	static Node* createNode(const nx::string& path)
	{
		auto node = new Node();
		node->nodeId = NodeIdCounter++;
		node->refcount = 1;

		// by default the name of the node is the last thing in the path bah
		if(path == "/") node->name == "/";
		else            node->name = splitPathComponents(path).back();
	}










	Status mount(DriverInterface* driver, const nx::string& mountpoint, bool readonly)
	{
		auto fs = new Filesystem();
		fs->driver = driver;
		fs->mountpoint = mountpoint;
		fs->filesystemId = FilesystemIdCounter++;
		fs->readOnly = readonly;

		MountedFilesystems.append(fs);

		return driver->init(fs);
	}

	File* open(const nx::string& path, Mode mode)
	{
	}

	void close(File* file)
	{
	}


	size_t read(File* file, void* buf, size_t count)
	{
	}

	size_t write(File* file, void* buf, size_t count)
	{
	}

	size_t seekAbs(File* file, size_t ofs)
	{
	}

	size_t seekRel(File* file, size_t ofs)
	{
	}

	Directory* openDir(const nx::string& path)
	{
	}

	void closeDir(Directory* dir)
	{
	}
}
}

























