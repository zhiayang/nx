// guids.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "efx.h"

#include "efi/protocol/file.h"
#include "efi/protocol/usb-io.h"
#include "efi/protocol/disk-io.h"
#include "efi/protocol/block-io.h"
#include "efi/protocol/device-path.h"
#include "efi/protocol/loaded-image.h"
#include "efi/protocol/simple-network.h"
#include "efi/protocol/driver-binding.h"
#include "efi/protocol/managed-network.h"
#include "efi/protocol/graphics-output.h"
#include "efi/protocol/simple-text-input.h"
#include "efi/protocol/simple-file-system.h"
#include "efi/protocol/simple-text-output.h"
#include "efi/protocol/pci-root-bridge-io.h"
#include "efi/protocol/device-path-to-text.h"

namespace efi {
namespace guid
{
	static efi_guid FileInfoGuid;
	static efi_guid UsbIoProtocol;
	static efi_guid DiskIoProtocol;
	static efi_guid BlockIoProtocol;
	static efi_guid FileSystemInfoGuid;
	static efi_guid DevicePathProtocol;
	static efi_guid LoadedImageProtocol;
	static efi_guid SimpleNetworkProtocol;
	static efi_guid DriverBindingProtocol;
	static efi_guid GraphicsOutputProtocol;
	static efi_guid ManagedNetworkProtocol;
	static efi_guid PciRootBridgeIoProtocol;
	static efi_guid SimpleTextInputProtocol;
	static efi_guid DevicePathToTextProtocol;
	static efi_guid SimpleFileSystemProtocol;
	static efi_guid SimpleTextOutputProtocol;
	static efi_guid DevicePathUtilitiesProtocol;

	void init()
	{
		FileInfoGuid = EFI_FILE_INFO_GUID;
		UsbIoProtocol = EFI_USB_IO_PROTOCOL_GUID;
		DiskIoProtocol = EFI_DISK_IO_PROTOCOL_GUID;
		BlockIoProtocol = EFI_BLOCK_IO_PROTOCOL_GUID;
		FileSystemInfoGuid = EFI_FILE_SYSTEM_INFO_GUID;
		DevicePathProtocol = EFI_DEVICE_PATH_PROTOCOL_GUID;
		LoadedImageProtocol = EFI_LOADED_IMAGE_PROTOCOL_GUID;
		SimpleNetworkProtocol = EFI_SIMPLE_NETWORK_PROTOCOL_GUID;
		DriverBindingProtocol = EFI_DRIVER_BINDING_PROTOCOL_GUID;
		GraphicsOutputProtocol = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
		ManagedNetworkProtocol = EFI_MANAGED_NETWORK_PROTOCOL_GUID;
		SimpleTextInputProtocol = EFI_SIMPLE_TEXT_INPUT_PROTOCOL_GUID;
		PciRootBridgeIoProtocol = EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GUID;
		SimpleFileSystemProtocol = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
		SimpleTextOutputProtocol = EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID;
		DevicePathToTextProtocol = EFI_DEVICE_PATH_TO_TEXT_PROTOCOL_GUID;
		DevicePathUtilitiesProtocol = EFI_DEVICE_PATH_UTILITIES_PROTOCOL_GUID;
	}

	efi_guid* fileInfo()                    { return &FileInfoGuid;                 }
	efi_guid* protoUSBIO()                  { return &UsbIoProtocol;                }
	efi_guid* protoDiskIO()                 { return &DiskIoProtocol;               }
	efi_guid* protoBlockIO()                { return &BlockIoProtocol;              }
	efi_guid* filesystemInfo()              { return &FileSystemInfoGuid;           }
	efi_guid* protoDevicePath()             { return &DevicePathProtocol;           }
	efi_guid* protoLoadedImage()            { return &LoadedImageProtocol;          }
	efi_guid* protoSimpleNetwork()          { return &SimpleNetworkProtocol;        }
	efi_guid* protoDriverBinding()          { return &DriverBindingProtocol;        }
	efi_guid* protoGraphicsOutput()         { return &GraphicsOutputProtocol;       }
	efi_guid* protoManagedNetwork()         { return &ManagedNetworkProtocol;       }
	efi_guid* protoPCIRootBrigeIO()         { return &PciRootBridgeIoProtocol;      }
	efi_guid* protoSimpleFilesytem()        { return &SimpleFileSystemProtocol;     }
	efi_guid* protoSimpleTextInput()        { return &SimpleTextInputProtocol;      }
	efi_guid* protoSimpleTextOutput()       { return &SimpleTextOutputProtocol;     }
	efi_guid* protoDevicePathToText()       { return &DevicePathToTextProtocol;     }
	efi_guid* protoDevicePathUtilities()    { return &DevicePathUtilitiesProtocol;  }



	efx::string tostring(const efi_guid& guid)
	{
		return efx::sprint("%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			guid.data1, guid.data2, guid.data3, guid.data4[0], guid.data4[1],
			guid.data4[2], guid.data4[3], guid.data4[4], guid.data4[5], guid.data4[6], guid.data4[7]
		);
	}


	efi_guid parse(const efx::string& str)
	{
		// this shit would be a lot easier if we had sscanf!
		if(str.size() != 36) efi::abort("malformed uuid '%s': expected length %zu, found %zu", 36, str.cstr(), str.size());

		// expect the hyphens at the correct positions.
		//         8    13   18   23
		// 123e4567-e89b-12d3-a456-426655440000

		if(str[8] != '-' || str[13] != '-' || str[18] != '-' || str[23] != '-')
			efi::abort("misplaced hyphens in uuid '%s'", str.cstr());

		auto bit1 = str.substring(0, 8);
		auto bit2 = str.substring(9, 4);
		auto bit3 = str.substring(14, 4);

		// ugh, explained below
		auto bit4 = str.substring(19, 2);
		auto bit5 = str.substring(21, 2);
		auto bit6 = str.substring(24, 2);
		auto bit7 = str.substring(26, 2);
		auto bit8 = str.substring(28, 2);
		auto bit9 = str.substring(30, 2);
		auto bitA = str.substring(32, 2);
		auto bitB = str.substring(34, 2);

		efi_guid ret;

		// because microsoft is dumb, the EFI guids have the first 3 'bits' little endian,
		// and the last 2 'bits' big-endian. stupid right? nbcb

		ret.data1 = strtol(bit1.cstr(), nullptr, 16);
		ret.data2 = strtol(bit2.cstr(), nullptr, 16);
		ret.data3 = strtol(bit3.cstr(), nullptr, 16);

		// ...
		ret.data4[0] = strtol(bit4.cstr(), nullptr, 16);
		ret.data4[1] = strtol(bit5.cstr(), nullptr, 16);
		ret.data4[2] = strtol(bit6.cstr(), nullptr, 16);
		ret.data4[3] = strtol(bit7.cstr(), nullptr, 16);
		ret.data4[4] = strtol(bit8.cstr(), nullptr, 16);
		ret.data4[5] = strtol(bit9.cstr(), nullptr, 16);
		ret.data4[6] = strtol(bitA.cstr(), nullptr, 16);
		ret.data4[7] = strtol(bitB.cstr(), nullptr, 16);

		return ret;
	}
}
}




























