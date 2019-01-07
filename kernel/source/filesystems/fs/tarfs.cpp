// tarfs.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

// implements a vfs driver for an in-memory TAR file.
// used for the initrd.

#include "nx.h"

namespace nx {
namespace vfs {
namespace tarfs
{
	struct tarent_t
	{
		// original format
		char name[100];
		char mode[8];
		char uid[8];
		char gid[8];
		char size[12];
		char mtime[12];
		char check[8];
		char link;
		char link_name[100];

		// ustar format
		char type;
		char also_link_name[100];
		char ustar[8];
		char owner[32];
		char group[32];
		char major[8];
		char minor[8];
		char name_prefix[155];
	};

	struct tarfile_t
	{
		string name;

		size_t offset;
		size_t size;
	};

	struct driverdata_t
	{
		uint8_t* buffer;
		size_t size;

		array<tarfile_t> files;
	};

	static size_t octalToNumber(char oct[12])
	{
		size_t ret = 0;
		int l = 12;

		int mul = 1;
		for(int i = l - 1; i-- > 0;)
		{
			ret += mul * (oct[i] - '0');
			mul *= 8;
		}

		return ret;
	}

	static int getCapabilities(Filesystem* fs)
	{
		return FSCAP_READ | FSCAP_SEEK;
	}

	static const char* getDescription(Filesystem* fs)
	{
		return "tarfs";
	}

	static bool init(Filesystem* fs)
	{
		// read the entire thing!!
		auto dd = (driverdata_t*) fs->driver->driverData;

		for(size_t i = 0; i < dd->size;)
		{
			if(dd->buffer[i] == 0)
			{
				i += 512;
				continue;
			}

			tarfile_t file;

			auto ent = (tarent_t*) (dd->buffer + i);
			file.name = string(ent->name_prefix) + string(ent->name);
			file.size = octalToNumber(ent->size);
			file.offset = i + 512;

			if(file.name[0] == '.')
				file.name.erase_at(0, 1);

			if(file.size > 0) dd->files.append(file);

			// nx::println("filename: %s, size: %zu", file.name.cstr(), file.size);

			auto s = ((file.size + 511) & ~511);
			i += __max(512, ((512 + s) / 512) * 512);
		}

		return true;
	}

	static bool openNode(Filesystem* fs, Node* node)
	{
		auto dd = (driverdata_t*) fs->driver->driverData;
		for(auto& f : dd->files)
		{
			if(f.name == node->path)
			{
				node->fsDriverData = &f;
				return true;
			}
		}

		return false;
	}

	static bool closeNode(Filesystem* fs, Node* node)
	{
		// we don't need to do anything
		return true;
	}

	static File* openFile(Filesystem* fs, Node* node, id_t fd, Mode mode)
	{
		auto file = new File();
		file->node = node;

		file->fileCursor = 0;
		file->openMode = mode;
		file->descriptorId = fd;

		assert(file->node);
		assert(file->node->fsDriverData);
		auto f = (tarfile_t*) file->node->fsDriverData;

		file->fileSize = f->size;
		return file;
	}

	static bool closeFile(Filesystem* fs, File* file)
	{
		delete file;
		return true;
	}

	static size_t read(Filesystem* fs, File* file, void* buf, size_t count)
	{
		auto dd = (driverdata_t*) fs->driver->driverData;
		auto fe = (tarfile_t*) file->node->fsDriverData;

		assert(dd);
		assert(fe);

		// ok, see how many bytes we can read from the file, taking into account the offset.
		size_t toread = __min(count, fe->size - fe->offset);

		memmove(buf, dd->buffer + fe->offset, toread);
		return toread;
	}






	DriverInterface* create(uint8_t* buf, size_t sz)
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
		ret->write              = 0;

		auto dd = new driverdata_t();
		dd->size = sz;
		dd->buffer = buf;

		ret->driverData = (void*) dd;

		return ret;
	}
}
}
}














