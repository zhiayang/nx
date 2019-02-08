// efx_main.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "efx.h"
#include "krt.h"

#include "efi/types.h"
#include "efi/system-table.h"
#include "efi/boot-services.h"

#include "efi/protocol/loaded-image.h"
#include "efi/protocol/graphics-output.h"

#define EFX_VERSION_STRING "0.4.0"


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

		loadKernel(buf, len, &kernelEntry);

		kernelBootInfo = prepareKernelBootInfo();

		// copy the kernel away.
		{
			uint64_t out = 0;
			auto stat = st->BootServices->AllocatePages(AllocateAnyPages, (efi_memory_type) efi::MemoryType_KernelElf,
				(len + 0x1000) / 0x1000, &out);
			efi::abort_if_error(stat, "failed to allocate memory for kernel!");

			memmove((void*) out, buf, len);


			kernelBootInfo->kernelElf = (void*) out;
			kernelBootInfo->kernelElfSize = len;
		}
	}


	if(auto path = options::get_option("initrd"); !path.empty())
	{
		// load an initrd!

		size_t len = 0;
		auto buf = fs::readFile(path, &len);

		// we need to copy it out as pages, since readFile() uses the EFI pool!
		{
			uint64_t out = 0;
			auto stat = st->BootServices->AllocatePages(AllocateAnyPages, (efi_memory_type) efi::MemoryType_Initrd,
				(len + 0x1000) / 0x1000, &out);
			efi::abort_if_error(stat, "failed to allocate memory for initrd!");

			memmove((void*) out, buf, len);
			st->BootServices->FreePool((void*) buf);

			buf = (uint8_t*) out;
		}

		kernelBootInfo->initrdSize = len;
		kernelBootInfo->initrdBuffer = buf;

		efi::println("loaded initrd %s, %s", path.cstr(), humanSizedBytes(len).cstr());
	}


	{
		efi::print("kernel parameters:");

		size_t reqSize = 0;

		// set kernel command line
		auto args = options::getKernelCommandLine();
		for(const auto& s : args)
		{
			// include null terminator
			reqSize += s.size() + 1;
			efi::print(" %s", s.cstr());
		}

		efi::println("");


		char* buffer = 0;
		{
			uint64_t out = 0;
			auto stat = st->BootServices->AllocatePages(AllocateAnyPages, (efi_memory_type) efi::MemoryType_BootInfo,
				(reqSize + 0x1000) / 0x1000, &out);
			efi::abort_if_error(stat, "failed to allocate memory for kernel parameters!");

			buffer = (char*) out;
		}


		// ok, start copying.
		size_t idx = 0;
		for(const auto& s : args)
		{
			memmove(buffer + idx, s.cstr(), s.size());
			idx += s.size();

			buffer[idx] = 0;
			idx += 1;
		}

		kernelBootInfo->kernelParams = buffer;
		kernelBootInfo->numKernelParams = args.size();
		kernelBootInfo->kernelParamsLength = reqSize;
	}



	{
		// before we go, allocate one last page for the efi memory map for runtime services.
		uint64_t scratch = 0;
		{
			auto stat = st->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderCode, 1, &scratch);
			efi::abort_if_error(stat, "failed to allocate page");
		}

		setKernelMemoryMap(kernelBootInfo);

		efi::println("\nexiting EFI boot services\n");
		efi::println("jumping to kernel; good luck!\n\n");

		memory::finaliseMappingExistingMemory();
		exitBootServices();

		memory::setEFIMemoryMap(kernelBootInfo, scratch);
		memory::installNewCR3();

		auto kernel_entry = (void (*)(nx::BootInfo*)) kernelEntry;
		kernel_entry(kernelBootInfo);
	}
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










