// vfs.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace vfs
{
	static nodecache* NodeCache;
	static array<Filesystem*> MountedFilesystems;

	static id_t NodeIdCounter = 0;
	static id_t FileDescCounter = 0;
	static id_t FilesystemIdCounter = 0;

	void init()
	{
		NodeCache = new nodecache();
		MountedFilesystems = array<Filesystem*>();

		FileDescCounter = 3;
	}

	static Filesystem* getFilesystemAtPath(const string& path)
	{
		auto bits = splitPathComponents(path);

		for(auto fs : MountedFilesystems)
		{
			auto fsm = splitPathComponents(fs->mountpoint);
			if(isPathSubset(bits, fsm))
				return fs;
		}

		return 0;
	}


	static Node* createNode(const string& path)
	{
		auto node = new Node();
		node->nodeId = NodeIdCounter++;
		node->refcount = 1;

		// by default the name of the node is the last thing in the path
		node->path = sanitise(path);
		node->filesystem = getFilesystemAtPath(node->path);

		return node;
	}








	bool mount(DriverInterface* driver, const string& mountpoint, bool readonly)
	{
		auto fs = new Filesystem();
		fs->driver = driver;
		fs->mountpoint = sanitise(mountpoint);
		fs->filesystemId = FilesystemIdCounter++;
		fs->readOnly = readonly;

		MountedFilesystems.append(fs);

		auto ret = driver->init(fs);
		if(ret) println("mounted %s at %s", driver->getDescription(fs), mountpoint.cstr());
		else    println("failed to mount %s at %s!", driver->getDescription(fs), mountpoint.cstr());

		return ret;
	}

	Node* openNode(const string& path)
	{
		auto n = NodeCache->fetch(path);
		if(n)
		{
			n->refcount += 1;
			return n;
		}
		else
		{
			n = createNode(path);
			NodeCache->insert(path, n);

			bool res = n->filesystem->driver->openNode(n->filesystem, n);
			if(!res)
			{
				println("failed to open node at path '%s'!", path.cstr());
				return nullptr;
			}

			return n;
		}
	}

	void closeNode(Node* node)
	{
		node->refcount -= 1;
		if(node->refcount == 0)
		{
			NodeCache->evict(node->path);
			delete node;
		}
	}



	File* open(const string& path, Mode mode)
	{
		// get the filesystem.
		auto fs = getFilesystemAtPath(path);
		if(!fs) println("no filesystem mounted at path '%s'!", path.cstr());

		// ok...
		return fs->driver->openFile(fs, openNode(path), FileDescCounter++, mode);
	}

	void close(File* file)
	{
		//* since fsdriver->openFile creates the File* for us,
		//* fsdriver->closeFile will delete it! so we store the Node pointer first.

		auto n = file->node;
		n->filesystem->driver->closeFile(n->filesystem, file);
		closeNode(n);
	}

	size_t read(File* file, void* buf, size_t count)
	{
		return 0;
	}

	size_t write(File* file, void* buf, size_t count)
	{
		return 0;
	}

	size_t seekAbs(File* file, size_t ofs)
	{
		return 0;
	}

	size_t seekRel(File* file, size_t ofs)
	{
		return 0;
	}

	Directory* openDir(const string& path)
	{
		return 0;
	}

	void closeDir(Directory* dir)
	{
	}
}
}

























