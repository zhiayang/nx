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
	}

	efi_guid* fileInfo()                { return &FileInfoGuid;              }
	efi_guid* protoUSBIO()              { return &UsbIoProtocol;             }
	efi_guid* protoDiskIO()             { return &DiskIoProtocol;            }
	efi_guid* protoBlockIO()            { return &BlockIoProtocol;           }
	efi_guid* filesystemInfo()          { return &FileSystemInfoGuid;        }
	efi_guid* protoDevicePath()         { return &DevicePathProtocol;        }
	efi_guid* protoLoadedImage()        { return &LoadedImageProtocol;       }
	efi_guid* protoSimpleNetwork()      { return &SimpleNetworkProtocol;     }
	efi_guid* protoDriverBinding()      { return &DriverBindingProtocol;     }
	efi_guid* protoGraphicsOutput()     { return &GraphicsOutputProtocol;    }
	efi_guid* protoManagedNetwork()     { return &ManagedNetworkProtocol;    }
	efi_guid* protoPCIRootBrigeIO()     { return &PciRootBridgeIoProtocol;   }
	efi_guid* protoSimpleFilesytem()    { return &SimpleFileSystemProtocol;  }
	efi_guid* protoSimpleTextInput()    { return &SimpleTextInputProtocol;   }
	efi_guid* protoSimpleTextOutput()   { return &SimpleTextOutputProtocol;  }
	efi_guid* protoDevicePathToText()   { return &DevicePathToTextProtocol;  }
}
}




























