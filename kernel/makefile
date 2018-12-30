# Makefile for Orion-X3/Orion-X4/mx and derivatives
# Written in 2011
# This makefile is licensed under the WTFPL


WARNINGS			= -Wno-padded -Wno-cast-align -Wno-unreachable-code -Wno-switch-enum -Wno-packed -Wno-missing-noreturn -Wno-float-equal -Wno-old-style-cast -Wno-unused-macros

CXXFLAGS			= -m64 -g -Wall -O2 -fno-omit-frame-pointer -fno-strict-aliasing -std=c++17 -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -Iinclude -I$(SYSROOT)/usr/include -DORION_KERNEL=1 -mcmodel=kernel -c


MEMORY				= 1024

SSRC				= $(shell find source -iname "*.s")
CXXSRC				= $(shell find source -iname "*.cpp")

SOBJ				= $(SSRC:.s=.s.o)
CXXOBJ				= $(CXXSRC:.cpp=.cpp.o)

LDFLAGS				= -z max-page-size=0x1000 -L$(SYSROOT)/usr/lib

CXXDEPS				= $(CXXOBJ:.o=.d)

.DEFAULT_GOAL = all
KERNEL				= nxkernel64

LIBM                = $(SYSROOT)/usr/lib/libm.a
LIBKRT              = $(SYSROOT)/usr/lib/libkrt.a


.PHONY: all

all: $(SYSROOT)/boot/$(KERNEL)

$(SYSROOT)/boot/$(KERNEL): $(SOBJ) $(CXXOBJ) $(LIBKRT) $(LIBM)
	@echo "# nxkernel"
	@$(LD) $(LDFLAGS) -T link.ld -o $@ $(SOBJ) $(CXXOBJ) $(LIBKRT) $(LIBM)

%.s.o: %.s
	@echo "# $(notdir $<)"
	@$(AS) $< -o $@

%.cpp.o: %.cpp
	@echo "# $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(WARNINGS) -MMD -MP -o $@ $<




-include $(CXXDEPS)













