# makefile
# Copyright (c) 2018, zhiayang
# Licensed under the Apache License Version 2.0.

export SYSROOT      := $(PROJECT_DIR)/build/sysroot
export CC           := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-gcc
export CXX          := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-g++
export AS           := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-as
export LD           := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-ld

export EFI_OBJCOPY  := $(PROJECT_DIR)/build/toolchain/bin/x86_64-w64-mingw32-objcopy

EFI_DIR     = zircon-efi
EFI_INCS    = -I$(EFI_DIR)

CXXSRC      = $(shell find source -name "*.cpp")
CSRC        = $(shell find source -name "*.c")
SSRC        = $(shell find source -name "*.s")

CSRC        += $(EFI_DIR)/platform/reloc_$(ARCH).c
SSRC        += $(EFI_DIR)/platform/crt0-efi-$(ARCH).s

COBJ        = $(CSRC:.c=.c.o)
CXXOBJ      = $(CXXSRC:.cpp=.cpp.o)
SOBJ        = $(SSRC:.s=.s.o)


COMMONFLAGS = -ffreestanding -maccumulate-outgoing-args -Iinclude -I$(PROJECT_DIR)/libs/libkrt/include -I$(EFI_DIR) -Wno-pragmas -O2 -fPIC -mno-red-zone -fno-exceptions -fshort-wchar -fno-strict-aliasing -MMD -MP

CFLAGS      = -std=c11 $(COMMONFLAGS)
CXXFLAGS    = -std=c++17 $(COMMONFLAGS)

LDFLAGS     = -nostdlib -Bsymbolic -shared -L $(SYSROOT)/usr/lib

LOADER      = $(SYSROOT)/boot/efxloader
LIBM        = $(SYSROOT)/usr/lib/libm.a
LIBKRT      = $(SYSROOT)/usr/lib/libkrt.a

CXXDEPS     = $(CXXOBJ:.o=.d)
CDEPS       = $(COBJ:.o=.d)



.PHONY: all

all: $(LOADER)

$(LOADER): $(COBJ) $(CXXOBJ) $(SOBJ) $(LIBKRT) $(LIBM)
	@echo "# $(notdir $@)"
	@$(LD) $(LDFLAGS) -T $(EFI_DIR)/platform/elf_$(ARCH)_efi.lds -o efxloader.tmp $(SOBJ) $(COBJ) $(CXXOBJ) -lkrt -lm
	@$(EFI_OBJCOPY) -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-x86_64 efxloader.tmp $@
	@rm efxloader.tmp

%.c.o: %.c
	@echo "# $(notdir $<)"
	@$(CC) $(CFLAGS) -c -o $@ $<

%.cpp.o: %.cpp
	@echo "# $(notdir $<)"
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

%.s.o: %.s
	@echo "# $(notdir $<)"
	@$(AS) $< -o $@

$(LIBM):
	@$(MAKE) -C $(PROJECT_DIR)/libs/libm

$(LIBKRT):
	@$(MAKE) -C $(PROJECT_DIR)/libs/libkrt






-include $(CXXDEPS) $(CDEPS)




