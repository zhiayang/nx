// filesystem.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "krt.h"
#include "efx.h"

#include "efi/system-table.h"
#include "efi/protocol/file.h"
#include "efi/protocol/device-path.h"
#include "efi/protocol/simple-file-system.h"

namespace efx {
namespace fs
{
	struct filesystem_t
	{
		efx::string label;
		uint64_t volumeSize;

		efi_file_protocol* fileProtocol = 0;
	};

	static efx::array<filesystem_t> filesystems;

	void discoverVolumes()
	{
		auto st = efi::systable();

		filesystems = efx::array<filesystem_t>();

		size_t num_handles = 0;
		efi_handle* handles = 0;

		auto stat = st->BootServices->LocateHandleBuffer(efi_locate_search_type::ByProtocol, efi::guid::protoSimpleFilesytem(), nullptr,
			&num_handles, &handles);

		efi::abort_if_error(stat, "failed to get handles! (%d)", stat);

		efi::println("found %zu filesystems:", num_handles);
		for(size_t i = 0; i < num_handles; i++)
		{
			efi::print("%zu: ", i);

			efi_simple_file_system_protocol* sfsproto = 0;
			stat = st->BootServices->HandleProtocol(handles[i], efi::guid::protoSimpleFilesytem(), (void**) &sfsproto);
			efi::abort_if_error(stat, "failed to open simple_fs_protocol for volume %zu", i);

			efi_file_protocol* fsroot = 0;
			stat = sfsproto->OpenVolume(sfsproto, &fsroot);
			efi::abort_if_error(stat, "failed to open volume %zu", i);

			filesystem_t fs;
			fs.fileProtocol = fsroot;

			{
				char buffer[256];
				size_t bufsize = 256;

				stat = fsroot->GetInfo(fsroot, efi::guid::filesystemInfo(), &bufsize, (void*) buffer);
				efi::abort_if_error(stat, "failed to get fsinfo for volume %zu", i);

				efi_file_system_info fsinfo;
				fsinfo = *((efi_file_system_info*) buffer);

				fs.volumeSize = fsinfo.VolumeSize;
			}

			{
				char buffer[256];
				size_t bufsize = 256;

				efi_guid guid = EFI_FILE_SYSTEM_VOLUME_LABEL_GUID;
				stat = fsroot->GetInfo(fsroot, &guid, &bufsize, (void*) buffer);
				efi::abort_if_error(stat, "failed to get volume label for volume %zu", i);

				fs.label = efx::string(efi::convertstr((char16_t*) buffer, 256));
			}

			efi::println("label(%s), size: %zu bytes", fs.label.data(), fs.volumeSize);
		}

		st->BootServices->FreePool(handles);
	}
}
}






























