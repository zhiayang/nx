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









