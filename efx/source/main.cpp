// main.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "efx.h"
#include "krt.h"

#include "efi/types.h"
#include "efi/system-table.h"
#include "efi/boot-services.h"

#include "efi/protocol/graphics-output.h"

extern "C" efi_status efi_main(efi_handle imageHandle, efi_system_table* sysTable)
{
	sysTable->ConOut->OutputString(sysTable->ConOut, STR("hello, world!!!\r\n"));
	// efiprintf("");

	// efi_graphics_output_protocol* gop = 0;
	// {
	// 	efi_guid guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	// 	auto stat = sysTable->BootServices->LocateProtocol(&guid, nullptr, (void**) &gop);
	// 	if(EFI_ERROR(stat))
	// 		sysTable->ConOut->OutputString(sysTable->ConOut, STR("failed to get GOP!\r\n"));
	// }

	// if(gop)
	// {
	// 	auto max_mode = gop->Mode->MaxMode;
	// 	for(uint32_t m = 0; m <= max_mode; m++)
	// 	{
	// 		size_t info_sz = 0;
	// 		efi_graphics_output_mode_information* modeinfo = 0;

	// 		gop->QueryMode(gop, m, &info_sz, &modeinfo);
	// 		// sysTable->ConOut->OutputString(sysTable->ConOut, STR("got a mode\r\n"));
	// 	}
	// }


	// sysTable->ConOut->OutputString(sysTable->ConOut, STR("hanging...\r\n"));
	while(true) {
	}
}










