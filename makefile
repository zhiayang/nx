# Makefile for Orion-X3/Orion-X4/mx and derivatives
# Written in 2011
# This makefile is licensed under the WTFPL

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


MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-builtin-variables

WARNINGS			= -Wno-padded -Wno-c++98-compat-pedantic -Wno-c++98-compat -Wno-cast-align -Wno-unreachable-code -Wno-gnu -Wno-missing-prototypes -Wno-switch-enum -Wno-packed -Wno-missing-noreturn -Wno-float-equal -Wno-old-style-cast -Wno-exit-time-destructors -Wno-unused-macros -Wno-global-constructors -Wno-documentation-unknown-command -Wno-reserved-id-macro -Wno-c99-extensions

CXXFLAGS			= -m64 -g -Weverything -msse3 -integrated-as -O2 -fno-omit-frame-pointer -fno-strict-aliasing -std=c++17 -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -I./source/Kernel/include -I./Libraries/Iris/HeaderFiles -I./Libraries/ -I$(SYSROOT)/usr/include -I$(SYSROOT)/usr/include/c++ -DORION_KERNEL=1 -target x86_64-elf -mcmodel=kernel -c

MEMORY				= 256

QEMU_UEFI_BIOS      = -bios utils/ovmf-x64/OVMF-pure-efi.fd
QEMU_DISK_IMAGE     = -drive format=raw,file=build/disk.img

QEMU_FLAGS          = -m $(MEMORY) $(QEMU_UEFI_BIOS) $(QEMU_DISK_IMAGE) -no-shutdown -no-reboot

QEMU_E9_PORT_STDIO  = -chardev stdio,id=qemu-debug-out -device isa-debugcon,chardev=qemu-debug-out
QEMU_E9_PORT_FILE   = -chardev file,id=qemu-debug-out,path=build/serialout.log -device isa-debugcon,chardev=qemu-debug-out


.DEFAULT_GOAL = all



.PHONY: all clean build diskimage qemu debug

all: qemu

debug: diskimage
	@echo -e "# starting qemu\n"
	@$(QEMU) $(QEMU_FLAGS) $(QEMU_E9_PORT_FILE) -vga std -d cpu_reset -monitor stdio

qemu: diskimage
	@echo -e "# starting qemu\n"
	@$(QEMU) $(QEMU_FLAGS) $(QEMU_E9_PORT_STDIO) -vga std




diskimage: build
	@utils/tools/update-diskimage.sh


build:
	@$(MAKE) -s -C libs/libc
	@$(MAKE) -s -C libs/libm
	@$(MAKE) -s -C libs/libkrt
	@$(MAKE) -s -C efx
	@$(MAKE) -s -C kernel

clean:
	@find "efx" -name "*.o" -delete
	@find "libs" -name "*.o" -delete
	@find "kernel" -name "*.o" -delete

	@find "efx" -name "*.cpp.d" -delete
	@find "libs" -name "*.cpp.d" -delete
	@find "kernel" -name "*.cpp.d" -delete









