# Makefile for Orion-X3/Orion-X4/mx and derivatives
# Written in 2011
# This makefile is licensed under the WTFPL


WARNINGS        = -Wno-padded -Wno-cast-align -Wno-unreachable-code -Wno-switch-enum -Wno-packed -Wno-missing-noreturn -Wno-float-equal -Wno-unused-macros # -pedantic-errors

CXXFLAGS        = -m64 -Wall -fno-omit-frame-pointer -fno-strict-aliasing -Wno-old-style-cast -std=c++17 -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -mcmodel=kernel -c


SSRC            =  $(shell find source -name "arch" -prune -o -name "*.s" -print)
SSRC            += $(shell find source/arch/$(ARCH) -iname "*.s")

CXXSRC          =  $(shell find source -name "arch" -prune -o -name "*.cpp" -print)
CXXSRC          += $(shell find source/arch/$(ARCH) -iname "*.cpp")


SOBJ            = $(SSRC:.s=.s.o)
CXXOBJ          = $(CXXSRC:.cpp=.cpp.o)

CXXDEPS         = $(CXXOBJ:.o=.d)

DEBUG_FLAG      := -msse2 -O0 -g
LDFLAGS         := -z max-page-size=0x1000 -L$(SYSROOT)/usr/lib

DEFINES         = -DORION_KERNEL=1 -D__ARCH_x64__=1
INCLUDES        = -Iinclude -I$(PROJECT_DIR)/libs/libkrt/include -I$(PROJECT_DIR)/libs/tinflate -I$(PROJECT_DIR)/efx/zircon-efi

KERNEL          = nxkernel64

LIBM            = $(SYSROOT)/usr/lib/libm.a
LIBKRT          = $(SYSROOT)/usr/lib/libkrt.a
LIBTINFLATE     = $(SYSROOT)/usr/lib/libtinflate.a

.PHONY: all
.DEFAULT_GOAL = all

UBSAN_FLAG      = -fsanitize=undefined




all: $(SYSROOT)/boot/$(KERNEL)
	@mkdir -p $(SYSROOT)/usr/include/nx
	@cp -r include/export/* $(SYSROOT)/usr/include/nx/

$(SYSROOT)/boot/$(KERNEL): $(SOBJ) $(COBJ) $(CXXOBJ) $(LIBKRT) $(LIBM) $(LIBTINFLATE)
	@echo "# nxkernel"
	@$(LD) $(LDFLAGS) -T link.ld -o $@ $(SOBJ) $(COBJ) $(CXXOBJ) -lkrt -lm -ltinflate

%.s.o: %.s
	@echo "# $(notdir $<)"
	@$(AS) -Isource/arch/$(ARCH)/include $< -o $@

%.cpp.o: %.cpp
	@echo "# $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(UBSAN_FLAG) $(WARNINGS) $(DEFINES) $(INCLUDES) $(DEBUG_FLAG) -MMD -MP -o $@ $<





-include $(CDEPS)
-include $(CXXDEPS)













