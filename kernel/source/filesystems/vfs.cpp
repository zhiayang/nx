// vfs.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace vfs
{
	static nodecache* NodeCache;
	static array<Filesystem*> MountedFilesystems;

	static id_t NodeIdCounter;
	static id_t FilesystemIdCounter;

	void init()
	{
		NodeCache = new nodecache();
		MountedFilesystems = array<Filesystem*>();
	}

	static bool isPathSubset(const array<string>& total, const array<string>& subset)
	{
		if(subset.size() > total.size()) return false;

		for(size_t i = 0; i < subset.size(); i++)
		{
			if(subset[i] != total[i])
				return false;
		}

		return true;
	}

	static Filesystem* getFilesystemAtPath(const string& path)
	{
		auto bits = splitPathComponents(path);
		for(auto fs : MountedFilesystems)
		{
			if(isPathSubset(bits, splitPathComponents(fs->mountpoint)))
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
		node->path = path;
		node->filesystem = getFilesystemAtPath(path);

		return node;
	}








	bool mount(DriverInterface* driver, const string& mountpoint, bool readonly)
	{
		auto fs = new Filesystem();
		fs->driver = driver;
		fs->mountpoint = mountpoint;
		fs->filesystemId = FilesystemIdCounter++;
		fs->readOnly = readonly;

		MountedFilesystems.append(fs);
		fs->rootNode = createNode("/");

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

			n->filesystem->driver->openNode(n->filesystem, n);
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
		return fs->driver->openFile(fs, openNode(path), mode);
	}

	void close(File* file)
	{
		file->node->filesystem->driver->closeFile(file->node->filesystem, file);
		closeNode(file->node);
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

























