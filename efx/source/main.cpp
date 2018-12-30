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

		fs::discoverVolumes(lip->DeviceHandle);
		fs::setRootFilesystemFromOpts();

		graphics::setDisplayMode(800, 600);
	}


	{
		auto kernelPath = options::get_option("kernel");
		if(kernelPath.empty()) efi::abort("did not specify 'kernel' option!");

		size_t len = 0;
		auto buf = fs::readFile(kernelPath, &len);

		// setup identity mapping first.
		// the kernel loading will call the appropriate functions to create virtual mappings.
		memory::setupCR3();

		uint64_t kernelEntry = 0;
		auto virts = loadKernel(buf, len, &kernelEntry);
	}





	/*
		things to do:

		6. create the memory map in a format that the kernel understands. we can probably stick to something rudimentary for now.

		8. jump to the kernel!

		7. exit boot services.
	 */


	efi::println("\nnothing to do...\n");
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










