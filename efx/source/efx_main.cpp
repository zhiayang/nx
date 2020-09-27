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

#define EFX_VERSION_STRING "0.5.3"


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

		// print the options
		auto loadopts = efx::string(efi::convertstr((char16_t*) lip->LoadOptions, lip->LoadOptionsSize));
		options::parse(loadopts);

		fs::discoverVolumes(lip->DeviceHandle);
		fs::setRootFilesystemFromOpts();
	}

	// these are defaults!!
	graphics::setDisplayMode(800, 600);


	size_t bootSvcKey = 0;
	uint64_t kernelEntry = 0;
	nx::BootInfo* kernelBootInfo = 0;
	{
		auto kernelPath = options::get_option("kernel");
		if(kernelPath.empty()) efi::abort("did not specify 'kernel' option!");


		efi::println("reading kernel from disk...");

		size_t len = 0;
		auto buf = fs::readFile(kernelPath, &len);

		// setup identity mapping first.
		// the kernel loading will call the appropriate functions to create virtual mappings.
		memory::setupCR3();

		loadKernel(buf, len, &kernelEntry);

		kernelBootInfo = prepareKernelBootInfo();

		// this is fixed, we always identity-map 4 mb.
		kernelBootInfo->maximumIdentityMap = 0x400000;

		// copy the kernel away.
		{
			auto out = efi::allocPages((len + 0xFFF) / 0x1000, efi::MemoryType_KernelElf, "kernel");
			memmove(out, buf, len);

			kernelBootInfo->kernelElf = out;
			kernelBootInfo->kernelElfSize = len;
		}
	}


	if(auto path = options::get_option("initrd"); !path.empty())
	{
		// load an initrd!
		efi::println("loading initrd...");

		size_t len = 0;
		auto buf = fs::readFile(path, &len);

		// we need to copy it out as pages, since readFile() uses the EFI pool!
		{
			auto pages = (len + 0xFFF) / 0x1000;
			auto out = efi::allocPages(pages, efi::MemoryType_Initrd, "initrd");

			memmove(out, buf, len);
			memset((uint8_t*) out + len, 0, pages * 0x1000 - len);

			auto crc32 = [](const uint8_t* ptr, size_t buf_len) -> uint32_t {
				static const uint32_t s_crc32[16] = {
					0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
					0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
					0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
					0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
				};

				uint32_t crcu32 = 0;
				if(!ptr) return 0;

				crcu32 = ~crcu32;
				while(buf_len--)
				{
					uint8_t b = *ptr++;
					crcu32 = (crcu32 >> 4) ^ s_crc32[(crcu32 & 0xF) ^ (b & 0xF)];
					crcu32 = (crcu32 >> 4) ^ s_crc32[(crcu32 & 0xF) ^ (b >> 4)];
				}
				return ~crcu32;
			};

			efi::println("initrd crc = %x", crc32((uint8_t*) out, len));

			st->BootServices->FreePool((void*) buf);
			buf = (uint8_t*) out;
		}

		kernelBootInfo->initrdSize = len;
		kernelBootInfo->initrdBuffer = buf;

		efi::println("loaded initrd %s, %zu bytes", path.cstr(), len);
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
		if(reqSize > 0)
		{
			auto out = efi::allocPages((reqSize + 0xFFF) / 0x1000, efi::MemoryType_BootInfo, "kernel parameters");
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
		auto scratch = (uint64_t) efi::allocPages(1, EfiLoaderCode, "runtime services");

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










