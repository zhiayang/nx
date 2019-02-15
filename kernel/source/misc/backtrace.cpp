// backtrace.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "elf.h"


#define NX_BOOTINFO_VERSION NX_SUPPORTED_BOOTINFO_VERSION
#include "bootinfo.h"


namespace nx {
namespace util
{
	nx::array<addr_t> getStackFrames(uint64_t _rbp)
	{
		array<addr_t> addrs;

		// fetch rbp
		addr_t rbp = (_rbp ? _rbp : (addr_t) __builtin_frame_address(0));

		do {
			auto rip = *((addr_t*) (rbp + sizeof(uint64_t)));
			if(!IS_CANONICAL(rip)) break;

			if(rip) addrs.append(rip);

			rbp = *((addr_t*) rbp);

		} while(rbp);

		return addrs;
	}


	struct Symbol
	{
		addr_t addr;
		size_t size;
		nx::string name;
	};

	static nx::array<Symbol> symbols;



	nx::string getSymbolAtAddr(addr_t x)
	{
		// loop through all symbols...
		for(const auto& sym : symbols)
		{
			if(x >= sym.addr && x < sym.addr + sym.size)
				return sym.name;
		}

		return "??";
	}


	void printStackTrace(uint64_t rbp)
	{
		serial::debugprintf("\nbacktrace:\n");
		if(!heap::initialised())
		{
			serial::debugprintf("    unavailable\n");
			return;
		}

		int counter = 0;
		auto frames = getStackFrames(rbp);
		for(auto frm : frames)
		{
			if(frm != 0)
			{
				serial::debugprintf("    %2d: %p   |   %s\n", counter, frm, getSymbolAtAddr(frm).cstr());
				counter += 1;
			}
		}
	}



	static void parseSymbolTable(Elf64_Sym* table, size_t numSyms, char* strings)
	{
		// index 0 is the undefined symbol.
		for(size_t i = 1; i < numSyms; i++)
		{
			if(table[i].st_value == 0 || table[i].st_name == 0) continue;

			// skip data for now
			if(ELF64_ST_TYPE(table[i].st_info) != STT_FUNC) continue;

			Symbol sym;

			sym.addr = table[i].st_value;
			sym.size = table[i].st_size;
			sym.name = demangleSymbol(string(&strings[table[i].st_name]));

			symbols.append(sym);
		}
	}



	void initSymbols(BootInfo* bi)
	{
		if(bi->version < 2)
		{
			warn("backtrace", "bootinfo version '%d' does not support symbol loading", bi->version);
			return;
		}

		// ok, do something.
		assert(bi->kernelElf);
		assert(bi->kernelElfSize);

		symbols = array<Symbol>();

		// make sure we can demangle shit.
		initDemangler();


		// load the elf file.
		auto file = (uint8_t*) bi->kernelElf;

		auto header = (Elf64_Ehdr*) file;
		{
			if(header->e_ident[EI_MAG0] != ELFMAG0 || header->e_ident[EI_MAG1] != ELFMAG1 || header->e_ident[EI_MAG2] != ELFMAG2
				|| header->e_ident[EI_MAG3] != ELFMAG3)
			{
				error("backtrace", "invalid ELF file! (wrong header)");
				return;
			}

			if(header->e_ident[EI_CLASS] != ELFCLASS64)
			{
				error("backtrace", "expected 64-bit ELF file!");
				return;
			}

			if(header->e_ident[EI_DATA] != ELFDATA2LSB)
			{
				error("backtrace", "big-endian ELFs not supported at this time!");
				return;
			}
		}



		{
			uint8_t* sectionNames = 0;
			// find the section string table first -- to get the names of our sections.
			{
				auto shstrtab = (Elf64_Shdr*) (file + header->e_shoff + (header->e_shstrndx * header->e_shentsize));
				assert(shstrtab->sh_type == SHT_STRTAB);

				sectionNames = file + shstrtab->sh_offset;
			}

			// (skip the first one cos it's always null)
			int strTabIdx = 0;
			for(int i = 1; i < header->e_shnum; i++)
			{
				auto sectionHdr = (Elf64_Shdr*) (file + header->e_shoff + (i * header->e_shentsize));

				// we only want the string table & the symbol table.
				if(strcmp((const char*) &sectionNames[sectionHdr->sh_name], ".strtab") == 0)
				{
					strTabIdx = i;
					break;
				}
			}

			assert(strTabIdx);


			char* strings = (char*) (file + ((Elf64_Shdr*) (file + header->e_shoff + (strTabIdx * header->e_shentsize)))->sh_offset);

			for(int i = 1; i < header->e_shnum; i++)
			{
				auto sectionHdr = (Elf64_Shdr*) (file + header->e_shoff + (i * header->e_shentsize));

				if(strcmp((const char*) &sectionNames[sectionHdr->sh_name], ".symtab") == 0)
					parseSymbolTable((Elf64_Sym*) (file + sectionHdr->sh_offset), (sectionHdr->sh_size / sectionHdr->sh_entsize), strings);
			}
		}


		log("backtrace", "found %s", util::plural("symbol", symbols.size()).cstr());

		pmm::freeEarlyMemory(bi, MemoryType::KernelElf);
	}
}
}





























