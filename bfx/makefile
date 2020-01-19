# makefile
# Copyright (c) 2018, zhiayang
# Licensed under the Apache License Version 2.0.

export SYSROOT      := $(PROJECT_DIR)/build/sysroot
export CC           := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-gcc
export CXX          := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-g++
export AS           := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-as
export LD           := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-ld

CXXSRC      = $(shell find source -name "*.cpp")
CSRC        = $(shell find source -name "*.c")
SSRC        = $(shell find source -name "*.s")

COBJ        = $(CSRC:.c=.c.o)
CXXOBJ      = $(CXXSRC:.cpp=.cpp.o)
SOBJ        = $(SSRC:.s=.s.o)

COMMONFLAGS = -ffreestanding -maccumulate-outgoing-args -Iinclude -I$(PROJECT_DIR)/libs/libkrt/include -I$(EFI_DIR) -Wno-pragmas -O2 -fPIC -mno-red-zone -fno-exceptions -fshort-wchar -fno-strict-aliasing -g -MMD -MP

CFLAGS      = -std=c11 $(COMMONFLAGS)
CXXFLAGS    = -std=c++17 $(COMMONFLAGS)

LOADER      = $(SYSROOT)/boot/bfxloader
LIBM        = $(SYSROOT)/usr/lib/libm.a
LIBKRT      = $(SYSROOT)/usr/lib/libkrt.a

CXXDEPS     = $(CXXOBJ:.o=.d)
CDEPS       = $(COBJ:.o=.d)


.PHONY: all

all: $(LOADER)

print-bfxloader:
	@echo "# bfxloader"

$(LOADER): $(COBJ) $(CXXOBJ) $(SOBJ) $(LIBKRT) $(LIBM)
	@$(LD) -nostdlib -nostartfiles -L $(SYSROOT)/usr/lib -o $@ -T link.ld $(COBJ) $(CXXOBJ) $(SOBJ) -lkrt -lm

%.c.o: %.c | print-bfxloader
	@echo "  $(notdir $<)"
	@$(CC) $(CFLAGS) -c -o $@ $<

%.cpp.o: %.cpp | print-bfxloader
	@echo "  $(notdir $<)"
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

%.s.o: %.s | print-bfxloader
	@echo "  $(notdir $<)"
	@$(AS) $< -o $@

$(LIBM):
	@$(MAKE) -C $(PROJECT_DIR)/libs/libm

$(LIBKRT):
	@$(MAKE) -C $(PROJECT_DIR)/libs/libkrt






-include $(CXXDEPS)
-include $(CDEPS)




