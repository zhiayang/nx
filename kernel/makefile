# Makefile for Orion-X3/Orion-X4/mx and derivatives
# Written in 2011
# This makefile is licensed under the WTFPL


WARNINGS        = -Wno-padded -Wno-cast-align -Wno-unreachable-code -Wno-switch-enum -Wno-packed -Wno-missing-noreturn -Wno-float-equal -Wno-unused-macros -pedantic-errors

CFLAGS          = -m64 -Wall -fno-omit-frame-pointer -fno-strict-aliasing -std=c11 -ffreestanding -mno-red-zone -Iinclude -I$(PROJECT_DIR)/libs/libkrt/include -I$(PROJECT_DIR)/libs/tinflate -DORION_KERNEL=1 -mcmodel=kernel -c

CXXFLAGS        = -m64 -Wall -fno-omit-frame-pointer -fno-strict-aliasing -Wno-old-style-cast -std=c++17 -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -Iinclude -I$(PROJECT_DIR)/libs/libkrt/include -I$(PROJECT_DIR)/libs/tinflate -DORION_KERNEL=1 -mcmodel=kernel -c


SSRC            =  $(shell find source -name "platform" -prune -o -name "*.s" -print)
SSRC            += $(shell find source/platform/$(ARCH) -iname "*.s")

CSRC            =  $(shell find source -name "platform" -prune -o -name "*.c" -print)
CSRC            += $(shell find source/platform/$(ARCH) -iname "*.c")

CXXSRC          =  $(shell find source -name "platform" -prune -o -name "*.cpp" -print)
CXXSRC          += $(shell find source/platform/$(ARCH) -iname "*.cpp")


SOBJ            = $(SSRC:.s=.s.o)
COBJ            = $(CSRC:.c=.c.o)
CXXOBJ          = $(CXXSRC:.cpp=.cpp.o)

CXXDEPS         = $(CXXOBJ:.o=.d)
CDEPS           = $(COBJ:.o=.d)

DEBUG_FLAG      := -O0 -g
LDFLAGS         := -z max-page-size=0x1000 -L$(SYSROOT)/usr/lib


KERNEL          = nxkernel64

LIBM            = $(SYSROOT)/usr/lib/libm.a
LIBKRT          = $(SYSROOT)/usr/lib/libkrt.a
LIBTINFLATE     = $(SYSROOT)/usr/lib/libtinflate.a

.PHONY: all
.DEFAULT_GOAL = all





all: $(SYSROOT)/boot/$(KERNEL)

$(SYSROOT)/boot/$(KERNEL): $(SOBJ) $(COBJ) $(CXXOBJ) $(LIBKRT) $(LIBM) $(LIBTINFLATE)
	@echo "# nxkernel"
	@$(LD) $(LDFLAGS) -T link.ld -o $@ $(SOBJ) $(COBJ) $(CXXOBJ) -lkrt -lm -ltinflate

%.s.o: %.s
	@echo "# $(notdir $<)"
	@$(AS) $< -o $@

%.cpp.o: %.cpp
	@echo "# $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(WARNINGS) $(DEBUG_FLAG) -MMD -MP -o $@ $<

%.c.o: %.c
	@echo "# $(notdir $<)"
	@$(CC) $(CFLAGS) $(WARNINGS) $(DEBUG_FLAG) -MMD -MP -o $@ $<





-include $(CDEPS)
-include $(CXXDEPS)













