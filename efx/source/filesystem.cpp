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
	};

	static efx::array<filesystem_t> filesystems;
	bool extractVolumeGuid(efi_handle handle, efi_guid* out);


	void discoverVolumes()
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

			efi::println("partition %zu (", i);
			efi::println("    label: %s", fs.label.data());
			efi::println("    guid:  %s", efi::guid::tostring(&fs.guid).data());
			efi::println("    size:  %s", efx::humanSizedBytes(fs.volumeSize).data());
			efi::println(")\n");
		}

		bs->FreePool(handles);
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
}
}






























