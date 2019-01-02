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

			Node* rootNode;


			void* fsDriverData;
			DriverInterface* driver;
		};

		struct Node
		{
			nx::string name;
			id_t nodeId;

			Filesystem* filesystem;

			size_t refcount;


			Flags flags;
			void* fsDriverData;

			Node* parent;

			// TODO:
			// timestamps for stuff
			// permissions
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


		void init();

		Status mount(DriverInterface* driver, const nx::string& mountpoint);

		File* open(const nx::string& path, Mode mode);
		void close(File* file);

		size_t read(File* file, void* buf, size_t count);
		size_t write(File* file, void* buf, size_t count);

		Directory* openDir(const nx::string& path);
		void closeDir(Directory* dir);

		size_t seekAbs(File* file, size_t ofs);
		size_t seekRel(File* file, size_t ofs);




		struct DriverInterface
		{
			// initialise the driver. it should not modify any of the existing fields except fsDriverData.
			Status (*init)(Filesystem* fs);

			// opens a file corresponding to the stuff in node, at the path specified.
			Status (*open)(Node* node, const nx::string& path);

			// closes the node. mainly to let the driver clean stuff up in fsDriverData.
			Status (*close)(Node* node);




			// opens a file for reading -- different from the thing above
			File* (*openFile)(Node* node, Mode mode);

			// closes the file.
			Status (*closeFile)(File* file);

			// reads a file. returns the number of bytes read.
			size_t (*read)(File* file, void* buf, size_t count);

			// writes to a file. returns the number of bytes written.
			size_t (*write)(File* file, void* buf, size_t count);

			// moves the cursor of a file -- relatively. returns the actual offset.
			size_t (*seekRelative)(File* file, size_t ofs);

			// same, but absolutely.
			size_t (*seekAbsolute)(File* file, size_t ofs);

			// TODO: create, delete, other stuff??
		};


	}
}























