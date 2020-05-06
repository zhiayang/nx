// symbols.cpp
// Copyright (c) 2020, zhiayang, Apache License 2.0.

#include "nx.h"
#include "elf.h"
#include "misc/util.h"

namespace nx::syms
{
	// big hack. big fucking hack.
	static bool didInit = false;
	static nx::string no_symbol;
	const nx::string& get_empty_sym()
	{
		if(!didInit)
			new (&no_symbol) nx::string("??");

		return no_symbol;
	}

	const nx::string& symbolicate(addr_t addr, scheduler::Process* proc)
	{
		assert(proc);
		auto& symlist = proc->symbols;

		// loop through all symbols...
		for(const auto& sym : symlist)
		{
			if(addr >= sym.addr && addr < sym.addr + sym.size)
				return sym.name;
		}

		// uwu
		return get_empty_sym();
	}


	static void parseSymbolTable(scheduler::Process* proc, Elf64_Sym* table, size_t numSyms, char* strings)
	{
		assert(proc);
		auto& symlist = proc->symbols;

		// index 0 is the undefined symbol.
		for(size_t i = 1; i < numSyms; i++)
		{
			if(table[i].st_value == 0 || table[i].st_name == 0) continue;

			// skip data for now
			if(ELF64_ST_TYPE(table[i].st_info) != STT_FUNC) continue;

			syms::Symbol sym;

			sym.addr = table[i].st_value;
			sym.size = table[i].st_size;
			sym.name = util::demangleSymbol(string(&strings[table[i].st_name]));

			symlist.append(sym);
		}
	}


	void readSymbolsFromELF(void* buf, size_t len, scheduler::Process* proc)
	{
		auto file = (uint8_t*) buf;
		auto header = (Elf64_Ehdr*) file;
		{
			if(header->e_ident[EI_MAG0] != ELFMAG0 || header->e_ident[EI_MAG1] != ELFMAG1 || header->e_ident[EI_MAG2] != ELFMAG2
				|| header->e_ident[EI_MAG3] != ELFMAG3)
			{
				error("syms", "invalid ELF file! (wrong header)");
				return;
			}

			if(header->e_ident[EI_CLASS] != ELFCLASS64)
			{
				error("syms", "expected 64-bit ELF file!");
				return;
			}

			if(header->e_ident[EI_DATA] != ELFDATA2LSB)
			{
				error("syms", "big-endian ELFs not supported at this time!");
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
					parseSymbolTable(proc, (Elf64_Sym*) (file + sectionHdr->sh_offset), (sectionHdr->sh_size / sectionHdr->sh_entsize), strings);
			}
		}
	}
}
