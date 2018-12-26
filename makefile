# Makefile for Orion-X3/Orion-X4/mx and derivatives
# Written in 2011
# This makefile is licensed under the WTFPL

export QEMU				:= qemu-system-x86_64
export SYSROOT			:= $(shell pwd)/build/sysroot
export TOOLCHAIN		:= $(shell pwd)/build/toolchain
export CC				:= $(shell pwd)/build/toolchain/bin/x86_64-orionx-gcc
export CXX				:= $(shell pwd)/build/toolchain/bin/x86_64-orionx-g++
export AS				:= $(shell pwd)/build/toolchain/bin/x86_64-orionx-as
export LD				:= $(shell pwd)/build/toolchain/bin/x86_64-orionx-ld
export OBJCOPY			:= $(shell pwd)/build/toolchain/bin/x86_64-orionx-objcopy
export READELF			:= $(shell pwd)/build/toolchain/bin/x86_64-orionx-readelf
export STRIP			:= $(shell pwd)/build/toolchain/bin/x86_64-orionx-strip
export AR				:= $(shell pwd)/build/toolchain/bin/x86_64-orionx-ar
export RANLIB			:= $(shell pwd)/build/toolchain/bin/x86_64-orionx-ranlib

export QEMU             := /mnt/d/programs/qemu/qemu-system-x86_64.exe

export ARCH             := x86_64
export PROJECT_DIR      := $(shell pwd)

# we use clang only for the kernel, don't pollute makefiles
CXX_				= clang++
FLXC				= $(SYSROOT)/usr/local/bin/flaxc
GCCVERSION			= 8.2.0

WARNINGS			= -Wno-padded -Wno-c++98-compat-pedantic -Wno-c++98-compat -Wno-cast-align -Wno-unreachable-code -Wno-gnu -Wno-missing-prototypes -Wno-switch-enum -Wno-packed -Wno-missing-noreturn -Wno-float-equal -Wno-old-style-cast -Wno-exit-time-destructors -Wno-unused-macros -Wno-global-constructors -Wno-documentation-unknown-command -Wno-reserved-id-macro -Wno-c99-extensions

CXXFLAGS			= -m64 -g -Weverything -msse3 -integrated-as -O2 -fno-omit-frame-pointer -fno-strict-aliasing -std=c++17 -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -I./source/Kernel/include -I./Libraries/Iris/HeaderFiles -I./Libraries/ -I$(SYSROOT)/usr/include -I$(SYSROOT)/usr/include/c++ -DORION_KERNEL=1 -target x86_64-elf -mcmodel=kernel -c

LDFLAGS				= -z max-page-size=0x1000 -L$(SYSROOT)/usr/lib

MEMORY				= 1024

SSRC				= $(shell find source -iname "*.s")
CXXSRC				= $(shell find source -iname "*.cpp")

SOBJ				= $(SSRC:.s=.s.o)
CXXOBJ				= $(CXXSRC:.cpp=.cpp.o)

CXXDEPS				= $(CXXOBJ:.o=.d)

QEMU_FLAGS          = -bios utils/ovmf-x64/OVMF-pure-efi.fd -drive format=raw,file=build/disk.img
QEMU_E9_PORT_STDIO  = -chardev stdio,id=qemu-debug-out -device isa-debugcon,chardev=qemu-debug-out

.DEFAULT_GOAL = all
-include $(CXXDEPS)

KERNEL				= $(SYSROOT)/boot/nxkernel64

.PHONY: all clean build libraries diskimage loader qemu

all: build qemu

build: libraries loader $(KERNEL)

$(KERNEL): $(SOBJ) $(CXXOBJ)

qemu: diskimage
	@echo -e "# starting qemu\n"
	@$(QEMU) $(QEMU_FLAGS) -vga std $(QEMU_E9_PORT_STDIO)

loader:
	@$(MAKE) -s -C efx all

libraries:
	@$(MAKE) -s -C libs/libc install
	@$(MAKE) -s -C libs/libm install
	@$(MAKE) -s -C libs/libkrt install

diskimage: build
	@utils/tools/update-diskimage.sh












%.s.o: %.s
	@$(AS) $< -o $@

%.cpp.o: %.cpp
	@$(CXX_) $(CXXFLAGS) $(WARNINGS) -MMD -MP -o $@ $<

clean:
	@find "libs" -name "*.o" -delete
	@find "source" -name "*.o" -delete

	@find "libs" -name "*.cpp.d" -delete
	@find "source" -name "*.cpp.d" -delete









