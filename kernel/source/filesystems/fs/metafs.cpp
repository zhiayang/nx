// metafs.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

// implements a meta filesystem, ie. operations on files in this fs
// will forward to the underlying filesystem.

#include "nx.h"

namespace nx {
namespace vfs {
namespace metafs
{
	struct driverdata_t
	{
		int _;
	};

	static int getCapabilities(Filesystem* fs)
	{
		return FSCAP_READ | FSCAP_WRITE;
	}

	static const char* getDescription(Filesystem* fs)
	{
		return "metafs";
	}

	static bool init(Filesystem* fs)
	{
		return true;
	}

	static bool openNode(Filesystem* fs, Node* node)
	{
		assert(fs);
		assert(node);

		if(node->path == "/dev/kernel_fbconsole")
			return true;

		return false;
	}

	static bool closeNode(Filesystem* fs, Node* node)
	{
		// we don't need to do anything
		return true;
	}

	static File* openFile(Filesystem* fs, Node* node, id_t fd, Mode mode)
	{
		// todo: check the mode, eg. don't write to stdin, don't read from stderr/stdout

		auto file = new File();
		file->node = node;

		file->fileCursor = 0;
		file->openMode = mode;
		file->descriptorId = fd;

		assert(file->node);
		assert(file->node->fsDriverData);

		file->fileSize = 0;
		return file;
	}

	static bool closeFile(Filesystem* fs, File* file)
	{
		delete file;
		return true;
	}

	static size_t read(Filesystem* fs, File* file, void* buf, size_t count)
	{
		// auto dd = (driverdata_t*) fs->driver->driverData;
		// auto fe = (tarfile_t*) file->node->fsDriverData;

		// assert(dd);
		// assert(fe);

		// // ok, see how many bytes we can read from the file, taking into account the offset.
		// size_t toread = __min(count, fe->size - file->fileCursor);

		// memmove(buf, dd->buffer + fe->offset, toread);
		// return toread;

		return 0;
	}

	static size_t write(Filesystem* fs, File* file, void* buf, size_t count)
	{
		if(!buf || !count)
			return 0;

		for(size_t i = 0; i < count; i++)
			console::putchar(((char*) buf)[i]);

		return count;
	}

	static bool stat(Filesystem* fs, File* file, Stat* st)
	{
		// auto fe = (tarfile_t*) file->node->fsDriverData;

		// st->fileSize = fe->size;
		// st->inodeNumber = fe->offset;
		// st->fileSizeIn512bBlocks = (st->fileSize + 512) / 512;

		// return true;

		return false;
	}





	DriverInterface* create()
	{
		auto ret = new DriverInterface();

		ret->getCapabilities    = getCapabilities;
		ret->getDescription     = getDescription;
		ret->init               = init;
		ret->openNode           = openNode;
		ret->closeNode          = closeNode;
		ret->openFile           = openFile;
		ret->closeFile          = closeFile;
		ret->read               = read;
		ret->write              = write;
		ret->stat               = stat;

		auto dd = new driverdata_t();
		ret->driverData = (void*) dd;

		return ret;
	}
}
}
}














