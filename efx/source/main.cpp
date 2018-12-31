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

#include "../../kernel/include/nx.h"

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

		efi::println("image base: %p\n", lip->ImageBase);

		// try and print the options bah
		auto loadopts = efx::string(efi::convertstr((char16_t*) lip->LoadOptions, lip->LoadOptionsSize));
		options::parse(loadopts);

		fs::discoverVolumes(lip->DeviceHandle);
		fs::setRootFilesystemFromOpts();

		graphics::setDisplayMode(800, 600);
	}

	size_t bootSvcKey = 0;
	uint64_t kernelEntry = 0;
	nx::BootInfo* kernelBootInfo = 0;
	{
		auto kernelPath = options::get_option("kernel");
		if(kernelPath.empty()) efi::abort("did not specify 'kernel' option!");

		size_t len = 0;
		auto buf = fs::readFile(kernelPath, &len);

		// setup identity mapping first.
		// the kernel loading will call the appropriate functions to create virtual mappings.
		memory::setupCR3();

		auto virts = loadKernel(buf, len, &kernelEntry);

		kernelBootInfo = prepareKernelBootInfo();
	}


	{
		// before we go, allocate one last page for the efi memory map for runtime services.
		uint64_t scratch = 0;
		{
			auto stat = st->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderCode, 1, &scratch);
			efi::abort_if_error(stat, "failed to allocate page");
		}

		efi::println("exiting EFI boot services");
		efi::println("jumping to kernel; good luck!\n\n");

		memory::finaliseMappingExistingMemory();
		exitBootServices();

		memory::setEFIMemoryMap(kernelBootInfo, scratch);
		memory::installNewCR3();

		auto kernel_entry = (void (*)(nx::BootInfo*)) kernelEntry;
		kernel_entry(kernelBootInfo);
	}



	efi::println("\nnothing to do...\n");
}



void efx::exitBootServices()
{
	auto bs = efi::systable()->BootServices;

	size_t mmKey = 0;
	{
		uint8_t buf = 0;
		size_t bufSz = 1;
		size_t descSz = 0;
		uint32_t descVer = 0;

		auto stat = bs->GetMemoryMap(&bufSz, (efi_memory_descriptor*) &buf, &mmKey, &descSz, &descVer);
		if(stat != EFI_BUFFER_TOO_SMALL) efi::abort_if_error(stat, "exitBootServices(): failed to get memory map");
	}

	auto stat = bs->ExitBootServices(efi::image_handle(), mmKey);
	efi::abort_if_error(stat, "failed to exit boot services?!");
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










