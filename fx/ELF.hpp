// ELF.hpp
// Copyright (c) 2011-2013, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.



#pragma once
#ifndef elf_h
#define elf_h

#include <stdint.h>



// ELF Magic Signature
#define ELF_MAGIC0		0x7F
#define ELF_MAGIC1		'E'
#define ELF_MAGIC2		'L'
#define ELF_MAGIC3		'F'
#define EI_NIDENT		16

#define EI_MAGIC0		0
#define EI_MAGIC1		1
#define EI_MAGIC2		2
#define EI_MAGIC3		3
#define EI_CLASS		4
#define EI_DATA		5
#define EI_VERSION		6
#define EI_OSABI		7
#define EI_ABIVERSION	8
#define EI_PAD			9

#define ElfClass32Bit		1
#define ElfClass64Bit		2
#define ElfDataLittleEndian	1


// Elf Types

#define Elf64_Addr 		uint64_t
#define Elf64_Off		uint64_t
#define Elf64_Half		uint16_t
#define Elf64_Word		uint32_t
#define Elf64_SWord		int32_t
#define Elf64_XWord		uint64_t
#define Elf64_SXWord	int64_t


// Elf Header Struct

struct __attribute__((packed)) ELF64FileHeader_type
{
	unsigned char		ElfIdentification[16];
	Elf64_Half		ElfType;
	Elf64_Half		ElfMachine;
	Elf64_Word		ElfVersion;
	Elf64_Addr		ElfEntry;
	Elf64_Off		ElfProgramHeaderOffset;
	Elf64_Off		ElfSectionHeaderOffset;
	Elf64_Word		ElfFlags;
	Elf64_Half		ElfHeaderSize;
	Elf64_Half		ElfProgramHeaderEntrySize;
	Elf64_Half		ElfProgramHeaderEntries;
	Elf64_Half		ElfSectionHeaderEntrySize;
	Elf64_Half		ElfSectionHeaderEntries;
	Elf64_Half		ElfStringTableIndex;

};

// Bunch of ElfType defines

#define ElfTypeNone			0	// No file type
#define ElfTypeRelocatable		1	// Relocatable File
#define ElfTypeExecutable		2	// Executable File
#define ElfTypeSharedObject		3	// Shared Object
#define ElfTypeCore			4	// Core File
#define ElfType_LOPROC		0xFF0	// Processor Specific
#define ElfType_HIPROC		0xFFF	// Processor Specific

// static constexpr const char* ElfFileType[] =
// {
// 	"None",
// 	"Relocatable",
// 	"Executable",
// 	"Shared Object",
// 	"Core File"
// };


// Bunch of ElfMachine defines

#define ElfMachineNone		0
#define ElfMachine386			3


// Bunch of ElfVersion defines

#define ElfVersionNone			0
#define ElfVersionCurrent		1



// Program Header Struct

struct __attribute__((packed)) ELF64ProgramHeader_type
{
	Elf64_Word			ProgramType;
	Elf64_Word			ProgramFlags;
	Elf64_Off			ProgramOffset;
	Elf64_Addr			ProgramVirtualAddress;
	Elf64_Addr			ProgramPhysicalAddress;
	Elf64_XWord			ProgramFileSize;
	Elf64_XWord			ProgramMemorySize;
	Elf64_XWord			ProgramAlign;

};


// Bunch of ProgramType defines

#define ProgramTypeNull			0
#define ProgramTypeLoadableSegment		1
#define ProgramTypeDynamicLinking		2
#define ProgramTypeIntepreter			3
#define ProgramTypeAuxillaryInfo		4
#define ProgramTypeSectionHeader		5
#define ProgramTypeProgramHeader		6
#define ProgramType_LOPROC			0x70000000
#define ProgramType_HIPROC			0x7FFFFFFF


// static constexpr const char* ElfSegmentType[] =
// {
// 	"Null",
// 	"Load",
// 	"Dynamic Link",
// 	"Intepreter",
// 	"Auxillary",
// 	"Section Header",
// 	"Program Header"
// };


// Section Header struct

struct __attribute__ ((__packed__))	ELF64SectionHeader_type
{
	Elf64_Word			SectionHeaderName;
	Elf64_Word			SectionHeaderType;
	Elf64_XWord			SectionHeaderFlags;
	Elf64_Addr			SectionHeaderAddress;
	Elf64_Off			SectionHeaderOffset;
	Elf64_XWord			SectionHeaderSize;
	Elf64_Word			SectionHeaderLink;
	Elf64_Word			SectionHeaderInfo;
	Elf64_XWord			SectionHeaderAddressAlign;
	Elf64_XWord			SectionHeaderEntrySize;

};


// Bunch of SectionHeaderType defines

#define SectionHeaderTypeNone		0
#define SectionHeaderTypeProgramBits	1
#define SectionHeaderTypeSymbolTable	2
#define SectionHeaderTypeStringTable		3
#define SectionHeaderTypeRelocation		4
#define SectionHeaderTypeHashTable		5
#define SectionHeaderTypeDynSymTable	6
#define SectionHeaderTypeNotes		7
#define SectionHeaderTypeNoBits		8

#define SHF_WRITE			(1 << 0)		// Writable
#define SHF_ALLOC			(1 << 1)		// Occupies memory during execution
#define SHF_EXECINSTR		(1 << 2)		// Executable
#define SHF_MERGE			(1 << 4)		// Might be merged
#define SHF_STRINGS			(1 << 5)		// Contains nul-terminated strings
#define SHF_INFO_LINK		(1 << 6)		// 'sh_info' contains SHT index
#define SHF_LINK_ORDER		(1 << 7)		// Preserve order after combining
#define SHF_OS_NONCONFORMING	(1 << 8)		// Non-standard OS specific handling required
#define SHF_GROUP			(1 << 9)		// Section is member of a group.
#define SHF_TLS			(1 << 10)	// Section hold thread-local data.



#endif
