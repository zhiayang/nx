// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <efi/types.h>

#define EFI_DEVICE_PATH_PROTOCOL_GUID \
	{ 0x09576e91, 0x6d3f, 0x11d2, { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } }

typedef struct efi_device_path_protocol
{
	uint8_t Type;
	uint8_t SubType;
	uint16_t Length;
} efi_device_path_protocol;


#define DEVICE_PATH_HARDWARE       0x01
#define DEVICE_PATH_ACPI           0x02
#define DEVICE_PATH_MESSAGING      0x03
#define DEVICE_PATH_MEDIA          0x04
#define DEVICE_PATH_BIOS_BOOT_SPEC 0x05
#define DEVICE_PATH_END            0x7f

#define DEVICE_PATH_INSTANCE_END 0x01
#define DEVICE_PATH_ENTIRE_END   0xff

#define DEVICE_PATH_HW_PCI        0x01
#define DEVICE_PATH_HW_PCCARD     0x02
#define DEVICE_PATH_HW_MEMMAP     0x03
#define DEVICE_PATH_HW_VENDOR     0x04
#define DEVICE_PATH_HW_CONTROLLER 0x05
#define DEVICE_PATH_HW_BMC        0x06














#define EFI_DEVICE_PATH_UTILITIES_PROTOCOL_GUID \
	{ 0x379be4e, 0xd706, 0x437d, { 0xb0, 0x37, 0xed, 0xb8, 0x2f, 0xb7, 0x72, 0xa4 } }

typedef struct efi_device_path_utilities_protocol
{
	uint64_t (*GetDevicePathSize) (
		struct efi_device_path_protocol* path
	) EFIAPI;

	struct efi_device_path_protocol* (*DuplicateDevicePath) (
		struct efi_device_path_protocol* devicepath
	) EFIAPI;

	struct efi_device_path_protocol* (*AppendDevicePath) (
		struct efi_device_path_protocol* src1,
		struct efi_device_path_protocol* src2
	) EFIAPI;

	struct efi_device_path_protocol* (*AppendDeviceNode) (
		struct efi_device_path_protocol* devicepath,
		struct efi_device_path_protocol* devicepathnode
	) EFIAPI;

	struct efi_device_path_protocol* (*AppendDevicePathInstance) (
		struct efi_device_path_protocol* path,
		struct efi_device_path_protocol* pathInstance
	) EFIAPI;

	struct efi_device_path_protocol* (*GetNextDevicePathInstance) (
		struct efi_device_path_protocol** pathInstance,
		uint64_t* instanceSize
	) EFIAPI;

	bool (*IsDevicePathMultiInstance) (
		struct efi_device_path_protocol* path
	) EFIAPI;

	struct efi_device_path_protocol* (*CreateDeviceNode) (
		uint8_t nodeType,
		uint8_t nodeSubType,
		uint16_t nodeLength
	) EFIAPI;

} efi_device_path_utilities_protocol;


// TODO: sub-types for other types (ACPI, etc)

// TODO: move this to another header? would break circular dependencies between
// boot-services.h and this header, for efi_memory_type
typedef struct
{
	efi_device_path_protocol Header;
	efi_memory_type MemoryType;
	efi_physical_addr StartAddress;
	efi_physical_addr EndAddress;
} efi_device_path_hw_memmap;




typedef struct efi_device_path_protocol efi_device_path_header;

typedef struct {
	efi_device_path_header header;
	uint32_t partitionNumber;
	uint64_t partitionStart;
	uint64_t partitionSize;
	uint8_t partitionGuid[16];
	uint8_t partitionFormat;
	uint8_t signatureType;
} efi_device_path_media_device;

typedef struct {
	efi_device_path_header header;
	uint8_t function;
	uint8_t device;
} efi_device_path_pci_device;

typedef struct {
	efi_device_path_header header;
	uint8_t function;
} efi_device_path_pccard;

typedef struct {
	efi_device_path_header header;
	uint8_t guid[16];
	uint8_t vendordata[];
} efi_device_path_vendor;

typedef struct {
	efi_device_path_header header;
	uint32_t controller;
} efi_device_path_controller;

typedef struct {
	efi_device_path_header header;
	uint8_t interfaceType;
	uint8_t baseAddress;
} efi_device_path_bmc;

typedef struct {
	efi_device_path_header header;
	uint32_t hid;
	uint32_t uid;
} efi_device_path_acpi;

typedef struct {
	efi_device_path_header header;
	uint32_t hid;
	uint32_t uid;
	uint32_t cid;

	// there's more stuff here:
	// char hidstr[]
	// char uidstr[]
	// char cidstr[]
} efi_device_path_acpi_expanded;

typedef struct {
	efi_device_path_header header;
	uint32_t adr;
	uint32_t additionalAdrs[];
} efi_device_path_acpi_adr;

typedef struct {
	efi_device_path_header header;
	uint8_t is_secondary;
	uint8_t is_slave;
	uint16_t logicalUnitNumber;
} efi_device_path_atapi;

typedef struct {
	efi_device_path_header header;
	uint16_t targetId;
	uint16_t logicalUnitNumber;
} efi_device_path_scsi;

typedef struct {
	efi_device_path_header header;
	uint32_t reserved;
	char worldwideName[8];
	uint64_t logicalUnitNumber;
} efi_device_path_fibre_channel;

typedef struct {
	efi_device_path_header header;
	uint32_t reserved;
	char worldwideName[8];
	char logicalUnitNumber[8];
} efi_device_path_fibre_channel_ex;

typedef struct {
	efi_device_path_header header;
	uint32_t reserved;
	uint64_t guid;
} efi_device_path_1394;

typedef struct {
	efi_device_path_header header;
	uint8_t parentPortNumber;
	uint8_t interfaceNumber;
} efi_device_path_usb;

typedef struct {
	efi_device_path_header header;
	uint16_t hbaPortNumber;
	uint16_t portMultiplierNumber;
	uint16_t logicalUnitNumber;
} efi_device_path_sata;

typedef struct {
	efi_device_path_header header;
	uint16_t interfaceNumber;
	uint16_t vendorId;
	uint16_t productId;
	char16_t serialNumber[];
} efi_device_path_usb_wwid;

typedef struct {
	efi_device_path_header header;
	uint8_t logicalUnitNumber;
} efi_device_path_device_logical_unit;

typedef struct {
	efi_device_path_header header;
	uint16_t vendorId;
	uint16_t productId;
	uint8_t deviceClass;
	uint8_t deviceSubclass;
	uint8_t deviceProtocol;
} efi_device_path_usb_class;

typedef struct {
	efi_device_path_header header;
	uint32_t targetId;
} efi_device_path_i2o;


