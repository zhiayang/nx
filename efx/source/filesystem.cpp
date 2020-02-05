// filesystem.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "krt.h"
#include "efx.h"

#include "string.h"

#include "efi/system-table.h"
#include "efi/protocol/file.h"
#include "efi/protocol/block-io.h"
#include "efi/protocol/device-path.h"
#include "efi/protocol/simple-file-system.h"
#include "efi/protocol/device-path-to-text.h"

namespace efx {
namespace fs
{
	struct filesystem_t
	{
		efi_guid guid;
		efx::string label;
		uint64_t volumeSize;

		efi_file_protocol* fileProtocol = 0;
		efi_block_io_protocol* blockProtocol = 0;

		// this tells us whether or not the filesystem is the one that we (efxloader) were loaded from.
		// if we did not get a 'root=...' load option, then we will attempt to find the kernel on this,
		// the de-facto root partition.
		bool deFactoRoot = false;
	};

	static efx::array<filesystem_t> filesystems;
	bool extractVolumeGuid(efi_handle handle, efi_guid* out);

	static filesystem_t* rootFilesystem = 0;



	void discoverVolumes(efi_handle currentHandle)
	{
		auto bs = efi::systable()->BootServices;

		filesystems = efx::array<filesystem_t>();

		size_t num_handles = 0;
		efi_handle* handles = 0;

		auto stat = bs->LocateHandleBuffer(efi_locate_search_type::ByProtocol, efi::guid::protoSimpleFilesytem(), nullptr,
			&num_handles, &handles);

		efi::abort_if_error(stat, "failed to get handles! (%d)", stat);

		efi::println("found %zu filesystems:", num_handles);
		for(size_t i = 0; i < num_handles; i++)
		{
			filesystem_t fs;

			{
				efi_simple_file_system_protocol* sfsproto = 0;
				stat = bs->HandleProtocol(handles[i], efi::guid::protoSimpleFilesytem(), (void**) &sfsproto);
				efi::abort_if_error(stat, "failed to open simple_fs_protocol for volume %zu", i);

				efi_file_protocol* fsroot = 0;
				stat = sfsproto->OpenVolume(sfsproto, &fsroot);
				efi::abort_if_error(stat, "failed to open volume %zu", i);

				fs.fileProtocol = fsroot;

				char buffer[256];
				size_t bufsize = 256;

				stat = fsroot->GetInfo(fsroot, efi::guid::filesystemInfo(), &bufsize, (void*) buffer);
				efi::abort_if_error(stat, "failed to get fsinfo for volume %zu", i);

				efi_file_system_info fsinfo;
				memcpy(&fsinfo, buffer, bufsize);

				fs.volumeSize = fsinfo.VolumeSize;
				fs.label = efx::string(efi::convertstr(fsinfo.VolumeLabel, 256));
			}


			// also get a blockio protocol for ourselves.
			{
				efi_block_io_protocol* blockproto = 0;
				stat = bs->HandleProtocol(handles[i], efi::guid::protoBlockIO(), (void**) &blockproto);
				efi::abort_if_error(stat, "failed to open blockio_protocol for volume %zu", i);

				efi_guid guid;
				bool found = extractVolumeGuid(handles[i], &guid);
				if(!found) efi::println("did not find guid for partition %zu", i);

				fs.guid = guid;
			}

			if(handles[i] == currentHandle)
				fs.deFactoRoot = true;

			efi::println("partition %zu (", i);
			efi::println("    label: %s", fs.label.data());
			efi::println("    guid:  %s", efi::guid::tostring(fs.guid).data());
			efi::println("    size:  %s", efx::humanSizedBytes(fs.volumeSize).data());
			efi::println(")\n");

			filesystems.append(fs);
		}

		bs->FreePool(handles);
	}


	void setRootFilesystemFromOpts()
	{
		auto opt = options::get_option("root");
		if(opt == "")
		{
			// then we use the de-facto root as the root.
			for(auto& fs : filesystems)
			{
				if(fs.deFactoRoot)
				{
					rootFilesystem = &fs;
					return;
				}
			}
		}
		else
		{
			// ok, we need to get the thing. we have two strategies -- if the name begins with "UUID",
			// then we search by UUID. else, we search by partition label.

			if(opt.find("UUID") == 0 || opt.find("GUID") == 0)
			{
				// make sure the option has 42 characters -- UUID(...), where ... is the 36-char long uuid
				if(opt.size() != 42) efi::abort("malformed uuid! expected format: UUID(123e4567-e89b-12d3-a456-426655440000)");

				auto uuidstr = opt.substring(4);
				if(uuidstr[0] != '(') efi::abort("expected '(' after UUID");
				if(uuidstr.back() != ')') efi::abort("expected closing ')'");

				auto uuid = efi::guid::parse(uuidstr.substring(1, 36));

				// loop through.
				for(auto& fs : filesystems)
					if(fs.guid == uuid) { rootFilesystem = &fs; return; }

				goto did_not_find;
			}
			else
			{
				for(auto& fs : filesystems)
					if(fs.label == opt){ rootFilesystem = &fs; return; }

				goto did_not_find;
			}
		}

	did_not_find:
		efi::abort("did not find root filesystem matching '%s'", opt.cstr());
	}





	bool extractVolumeGuid(efi_handle handle, efi_guid* out_guid)
	{
		// grab the device path thingy
		auto bs = efi::systable()->BootServices;

		efi_device_path_protocol* devicepath = 0;
		auto stat = bs->HandleProtocol(handle, efi::guid::protoDevicePath(), (void**) &devicepath);
		efi::abort_if_error(stat, "failed to get device path");

		// get the size, i guess?
		uint64_t pathsize = 0;
		{
			efi_device_path_utilities_protocol* dpu = 0;
			stat = bs->LocateProtocol(efi::guid::protoDevicePathUtilities(), nullptr, (void**) &dpu);
			efi::abort_if_error(stat, "failed to get device path utilities protocol");

			pathsize = dpu->GetDevicePathSize(devicepath);
		}

		// i'm not entirely sure how we're supposed to handle this, but i'm just gonna say fuck it
		// time for some raw pointer manipulation. we don't want to handle every type of path node, so we just use
		// the generic header, while maintaining a raw pointer.

		auto ptr = (uint8_t*) devicepath;
		uint64_t doneSz = 0;

		while(doneSz < pathsize)
		{
			auto hdr = (efi_device_path_header*) ptr;
			if(hdr->Type == DEVICE_PATH_END)
			{
				break;
			}
			else if(hdr->Type == DEVICE_PATH_MEDIA)
			{
				// we only care about this one -- specifically the guid!
				auto node = (efi_device_path_media_device*) hdr;
				memcpy(&out_guid->data[0], &node->partitionGuid[0], 16);
				return true;
			}

			ptr += hdr->Length;
			doneSz += hdr->Length;
		}

		// if we reached here, we did not find a guid!
		return false;
	}




	uint8_t* readFile(const efx::string& path, size_t* size)
	{
		auto issep = [](char c) -> bool { return c == '/' || c == '\\'; };

		if(path.empty()) efi::abort("readFile(): empty path!");
		if(!rootFilesystem) efi::abort("readFile(): no root filesystem!");
		if(issep(path.back())) efi::abort("readFile(): cannot call readFile on a folder");

		// we do not support the concept of a 'current directory'. if the path does not start with '/', assume it starts with '/' anyway.
		// we support spaces in paths by virtue of not caring about them.
		// also we support either / or \ as separators.

		// this also means that we cannot read files that are not on the root directory
		// but who needs that shit anyway?

		// skip any leading separators.
		size_t i = 0;
		while(issep(path[i]))
			i++;

		efx::string filename;
		efx::array<efx::string> directories;

		// ok, now the first piece should be the folder we want.

		for(; i < path.size(); i++)
		{
			if(issep(path[i]))
			{
				directories.append(filename);
				filename.clear();
			}
			else
			{
				filename.append(path[i]);
			}
		}

		// ok, because of the way we iterate, 'directories' should contain everything but the last thing,
		// and filename is the last thing. ezpz

		efi_file_protocol* cur = rootFilesystem->fileProtocol;
		for(const auto& dir : directories)
		{
			efi_file_protocol* handle = 0;
			auto stat = cur->Open(cur, &handle, efi::convertstr(dir.cstr(), dir.size()), EFI_FILE_MODE_READ, 0);
			if(stat == EFI_NOT_FOUND) efi::abort("readFile(): did not find folder '%s'", dir.cstr());
			efi::abort_if_error(stat, "readFile(): error opening folder '%s'", dir.cstr());

			cur = handle;
		}

		// ok, now we need to open the file itself.
		size_t filesz = 0;
		efi_file_protocol* file = 0;
		{
			auto stat = cur->Open(cur, &file, efi::convertstr(filename.cstr(), filename.size()), EFI_FILE_MODE_READ, 0);
			if(stat == EFI_NOT_FOUND) efi::abort("readFile(): did not find file '%s'", filename.cstr());
			efi::abort_if_error(stat, "readFile(): error opening file '%s'", filename.cstr());

			char buffer[256]; size_t bufsize = 256;

			stat = file->GetInfo(file, efi::guid::fileInfo(), &bufsize, (void*) buffer);
			efi::abort_if_error(stat, "readFile(): failed to get file info file '%s'", filename.cstr());

			efi_file_info finfo;
			memcpy(&finfo, buffer, bufsize);

			if(finfo.Attribute & EFI_FILE_DIRECTORY)
				efi::abort("readFile(): '%s' was supposed to be a file, but is actually a directory!", filename.cstr());

			filesz = finfo.FileSize;
		}

		{
			void* buf = efi::alloc(filesz); size_t outbytes = filesz;
			auto stat = file->Read(file, &outbytes, buf);
			efi::abort_if_error(stat, "readFile(): error reading file '%s'", filename.cstr());

			if(outbytes != filesz) efi::abort("readFile(): mismatched size! expected %zu bytes, but only %zu were read", filesz, outbytes);

			// contents = efx::string((char*) buf, filesz);
			*size = filesz;
			return (uint8_t*) buf;
		}
	}

	efx::string readFile(const efx::string& path)
	{
		size_t sz = 0;
		auto buf = readFile(path, &sz);

		return efx::string((char*) buf, sz);
	}
}
}






























