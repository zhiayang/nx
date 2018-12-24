# makefile
# Copyright (c) 2018, zhiayang
# Licensed under the Apache License Version 2.0.

export SYSROOT      := $(PROJECT_DIR)/build/sysroot
export CC           := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-gcc
export CXX          := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-g++
export AS           := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-as
export LD           := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-ld

export EFI_OBJCOPY  := $(PROJECT_DIR)/build/toolchain/bin/x86_64-w64-mingw32-objcopy

EFI_DIR     = $(PROJECT_DIR)/libs/zircon-efi
EFI_INCS    = -I$(EFI_DIR)

CXXSRC      = $(shell find . -name "*.cpp")
CSRC        = $(shell find . -name "*.c") $(EFI_DIR)/platform/reloc_$(ARCH).c
_SSRC       = $(shell find . -name "*.s") $(EFI_DIR)/platform/crt0-efi-$(ARCH).s
SSRC        = $(_SSRC:.S=.s)

COBJ        = $(CSRC:.c=.c.o)
CXXOBJ      = $(CXXSRC:.cpp=.cpp.o)
SOBJ        = $(SSRC:.s=.s.o)


COMMONFLAGS = -ffreestanding -maccumulate-outgoing-args -Iinclude -I$(PROJECT_DIR)/libs/libkrt/include -I$(EFI_DIR) -Wno-pragmas -O2 -fPIC -mno-red-zone -fno-exceptions -fshort-wchar

CFLAGS      =  $(COMMONFLAGS) -std=c11
CXXFLAGS    =  $(COMMONFLAGS) -std=c++17

LDFLAGS     = -nostdlib -Bsymbolic -shared -L $(PROJECT_DIR)/libs/libkrt

LOADER      = $(SYSROOT)/boot/efxloader

.PHONY: all clean


all: $(LOADER)

$(LOADER): $(COBJ) $(CXXOBJ) $(SOBJ)
	echo "# $(notdir $@)"
	@$(LD) $(LDFLAGS) -T $(EFI_DIR)/platform/elf_$(ARCH)_efi.lds -o efxloader.tmp $(SOBJ) $(COBJ) $(CXXOBJ) -lkrt
	@$(EFI_OBJCOPY) -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-x86_64 efxloader.tmp $@
	@rm efxloader.tmp

%.c.o: %.c
	@$(CC) $(CFLAGS) -c -DEFI_FUNCTION_WRAPPER -o $@ $<

%.cpp.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c -fshort-wchar -DEFI_FUNCTION_WRAPPER -o $@ $<

%.s.o: %.s
	@$(AS) $< -o $@




clean:
	@find . -name "*.o" -delete









