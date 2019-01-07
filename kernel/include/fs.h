// fs.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "defs.h"
#include <sys/types.h>

namespace nx
{
	namespace vfs
	{
		enum class Flags
		{
			File            = 0x01,
			Folder          = 0x02,
			Symlink         = 0x04,
			RawDevice       = 0x08,
			MountPoint      = 0x10,
		};

		struct Node;
		struct DriverInterface;

		struct Filesystem
		{
			id_t filesystemId;
			nx::string description;

			bool readOnly;
			nx::string mountpoint;

			DriverInterface* driver;
		};

		struct Stat
		{
			size_t fileSize;
			size_t fileSizeIn512bBlocks;
			ino_t inodeNumber;
		};

		struct Node
		{
			nx::string path;
			id_t nodeId;

			Filesystem* filesystem;

			size_t refcount;

			Flags flags;
			void* fsDriverData;

			// permissions
			// timestamps
		};

		enum class Mode
		{
			Read,
			Write,
			Append
		};

		struct File
		{
			id_t descriptorId;

			Node* node;
			Mode openMode;

			size_t fileSize;
			size_t fileCursor;
		};

		struct Directory
		{
			id_t descriptorId;

			Node* node;
			nx::array<Node*> children;
		};


		enum class Status
		{
			Success,

			ReadOnly,           // trying to modify a read-only thing
			NonExistent,        // does not exist
		};

		static constexpr int FSCAP_READ     = 0x01;
		static constexpr int FSCAP_WRITE    = 0x02;
		static constexpr int FSCAP_SEEK     = 0x04;
		static constexpr int FSCAP_DIR      = 0x08;
		static constexpr int FSCAP_DELETE   = 0x10;


		void init();

		bool mount(DriverInterface* driver, const nx::string& mountpoint, bool readonly);

		Node* openNode(const nx::string& path);
		void closeNode(Node* node);

		File* open(const nx::string& path, Mode mode);
		void close(File* file);

		size_t read(File* file, void* buf, size_t count);
		size_t write(File* file, void* buf, size_t count);

		Stat stat(File* file);

		Directory* openDir(const nx::string& path);
		void closeDir(Directory* dir);

		size_t seekAbs(File* file, size_t ofs);
		size_t seekRel(File* file, size_t ofs);




		struct DriverInterface
		{
			// returns the capabilities, as indicated by the FSCAP bitflags.
			int (*getCapabilities)(Filesystem* fs);

			const char* (*getDescription)(Filesystem* fs);

			// initialise the driver. it should not modify any of the existing fields except fsDriverData.
			bool (*init)(Filesystem* fs);

			// opens a file corresponding to the stuff in node, at the path specified.
			// it should not modify any of the existing fields except fsDriverData.
			bool (*openNode)(Filesystem* fs, Node* node);

			// closes the node. mainly to let the driver clean stuff up in fsDriverData.
			bool (*closeNode)(Filesystem* fs, Node* node);




			// opens a file
			File* (*openFile)(Filesystem* fs, Node* node, id_t fd, Mode mode);

			// closes the file.
			bool (*closeFile)(Filesystem* fs, File* file);

			// reads a file. returns the number of bytes read.
			size_t (*read)(Filesystem* fs, File* file, void* buf, size_t count);

			// writes to a file. returns the number of bytes written.
			size_t (*write)(Filesystem* fs, File* file, void* buf, size_t count);

			// gets some stats.
			bool (*stat)(Filesystem* fs, File* file, Stat* out);

			void* driverData = 0;

			// TODO: create, delete, other stuff??
		};



		// helper functions
		nx::array<nx::string> splitPathComponents(const nx::string& path);
		nx::string concatPath(const nx::array<nx::string>& components);
		nx::string sanitise(const nx::string& path);

		bool isPathSubset(const nx::array<nx::string>& total, const nx::array<nx::string>& subset);
		nx::array<nx::string> getFSRelativePath(Filesystem* fs, const nx::array<nx::string>& components);

		struct nodecache
		{
			nodecache() : count(0) { }

			Node* fetch(const nx::string& path);
			void insert(const nx::string& path, Node* node);

			void evict(const nx::string& path);

			size_t size();
			void clear();


			private:
			size_t count;
			nx::array<nx::treemap<nx::string, Node*>> store;
		};


		namespace tarfs
		{
			DriverInterface* create(uint8_t* buf, size_t sz);
		}
	}
}























