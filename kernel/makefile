# Makefile for Orion-X3/Orion-X4/mx and derivatives
# Written in 2011
# This makefile is licensed under the WTFPL



WARNINGS			= -Wno-padded -Wno-c++98-compat-pedantic -Wno-c++98-compat -Wno-cast-align -Wno-unreachable-code -Wno-gnu -Wno-missing-prototypes -Wno-switch-enum -Wno-packed -Wno-missing-noreturn -Wno-float-equal -Wno-old-style-cast -Wno-exit-time-destructors -Wno-unused-macros -Wno-global-constructors -Wno-documentation-unknown-command -Wno-reserved-id-macro -Wno-c99-extensions

CXXFLAGS			= -m64 -g -Weverything -msse3 -integrated-as -O2 -fno-omit-frame-pointer -fno-strict-aliasing -std=c++17 -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -I./source/Kernel/include -I./Libraries/Iris/HeaderFiles -I./Libraries/ -I$(SYSROOT)/usr/include -I$(SYSROOT)/usr/include/c++ -DORION_KERNEL=1 -target x86_64-elf -mcmodel=kernel -c

LDFLAGS				= -z max-page-size=0x1000 -L$(SYSROOT)/usr/lib

MEMORY				= 1024

SSRC				= $(shell find source -iname "*.s")
CXXSRC				= $(shell find source -iname "*.cpp")

SOBJ				= $(SSRC:.s=.s.o)
CXXOBJ				= $(CXXSRC:.cpp=.cpp.o)

CXXDEPS				= $(CXXOBJ:.o=.d)

-include $(CXXDEPS)

.DEFAULT_GOAL = all
KERNEL				= nxkernel64

LIBM                = $(SYSROOT)/usr/lib/libm.a
LIBKRT              = $(SYSROOT)/usr/lib/libkrt.a



.PHONY: all build install

install: all
	@cp $(KERNEL) $(SYSROOT)/boot/

all: build

build: $(KERNEL)

$(KERNEL): $(SOBJ) $(CXXOBJ) $(LIBKRT) $(LIBM)

%.s.o: %.s
	@echo "# $(notdir $<)"
	@$(AS) $< -o $@

%.cpp.o: %.cpp
	@echo "# $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(WARNINGS) -MMD -MP -o $@ $<

















