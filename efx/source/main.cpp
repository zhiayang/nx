// main.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "efx.h"
#include "krt.h"

#include "efi/types.h"
#include "efi/system-table.h"
#include "efi/boot-services.h"

#include "efi/protocol/loaded-image.h"
#include "efi/protocol/graphics-output.h"

#define EFX_VERSION_STRING "0.1.1"


void efx::init()
{
	auto st = efi::systable();

	efi::println("efx bootloader");
	efi::println("version %s\n", EFX_VERSION_STRING);

	{
		efi_loaded_image_protocol* lip = 0;
		auto s = st->BootServices->HandleProtocol(efi::image_handle(), efi::guid::protoLoadedImage(), (void**) &lip);
		efi::abort_if_error(s, "could not find LoadedImageProtocol, required to parse boot options");

		// try and print the options bah
		auto loadopts = efx::string(efi::convertstr((char16_t*) lip->LoadOptions, lip->LoadOptionsSize));
		options::parse(loadopts);

		efi::println("");
	}


	{
		// discover all volumes.
		fs::discoverVolumes();
	}




	/*
		things to do:

		3. find the kernel
			- since this file (efxloader) is on the root partition (not the ESP), it means that rEFInd managed
			  to read the FS, meaning that it should have loaded the FS driver to EFI.
			- figure out how to use EFI to read files.

		4. potentially set a graphics mode
			- probably not a high priority

		5. start mapping virtual memory:
			- load the kernel at a fixed physical address. add that to our memory map with an entry that says "yo this is you"
			- map the physical kernel to the higher half
			- both of these should be able to be done using efi boot services.
			- the kernel can copy itself to a new physical location and re-map the virtual pages to point
			  there, if we really want to.

		6. create the memory map in a format that the kernel understands. we can probably stick to something rudimentary for now.

		7. exit boot services.

		8. jump to the kernel!
	 */
}

































extern "C" efi_status efi_main(efi_handle imageHandle, efi_system_table* sysTable)
{
	asm volatile("cli");


	efi::guid::init();
	efi::init(imageHandle, sysTable);

	sysTable->ConOut->ClearScreen(sysTable->ConOut);

	efx::init();

	while(true)
		;
}










