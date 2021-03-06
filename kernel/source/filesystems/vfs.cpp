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

		scheduler::getKernelProcess()->ioctx = new vfs::IOCtx();
	}

	static Filesystem* getFilesystemAtPath(const array<string>& pathcomps)
	{
		for(auto fs : MountedFilesystems)
		{
			auto fsm = splitPathComponents(fs->mountpoint);
			if(isPathSubset(pathcomps, fsm))
				return fs;
		}

		return 0;
	}

	static Node* createNode(const string& path)
	{
		auto node = new Node();
		node->nodeId = NodeIdCounter++;
		node->refcount = 1;

		auto comps = splitPathComponents(sanitise(path));

		node->filesystem = getFilesystemAtPath(comps);
		node->path = concatPath(getFSRelativePath(node->filesystem, comps));

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
		if(ret) log("vfs", "mounted {} at {}", driver->getDescription(fs), mountpoint);
		else    error("vfs", "failed to mount {} at {}!", driver->getDescription(fs), mountpoint);

		return ret;
	}

	Node* openNode(IOCtx* ctx, const string& path)
	{
		assert(ctx);

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
				error("vfs", "failed to open node at path '{}'!", path);
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



	File* open(IOCtx* ctx, const string& path, Mode mode)
	{
		assert(ctx);

		// get the filesystem.
		auto fs = getFilesystemAtPath(splitPathComponents(path));
		if(!fs) { error("vfs", "no filesystem mounted at path '{}'!", path); return nullptr; }

		// ok...
		auto nd = openNode(ctx, path);
		if(!nd) return nullptr;

		auto file = fs->driver->openFile(fs, nd, FileDescCounter++, mode);
		if(file) ctx->openFiles.append(file);

		return file;
	}

	// note: the number of indirections in these methods makes me cry a bit ):
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
		if(!file) return 0;
		if(!buf || !count) return 0;

		assert(file->node);
		assert(file->node->filesystem);

		return file->node->filesystem->driver->read(file->node->filesystem, file, buf, count);
	}

	size_t write(File* file, void* buf, size_t count)
	{
		if(!file) return 0;
		if(!buf || !count) return 0;

		assert(file->node);
		assert(file->node->filesystem);

		return file->node->filesystem->driver->write(file->node->filesystem, file, buf, count);
	}

	Stat stat(File* file)
	{
		Stat ret;
		if(file)
		{
			assert(file->node);
			assert(file->node->filesystem);

			file->node->filesystem->driver->stat(file->node->filesystem, file, &ret);
		}
		else
		{
			error("vfs", "file was null!");
		}

		return ret;
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

























