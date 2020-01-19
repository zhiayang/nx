# Makefile for Orion-X3/Orion-X4/mx and derivatives
# Written in 2011
# This makefile is licensed under the WTFPL

export PROJECT_DIR      := $(shell pwd)

export SYSROOT          := $(PROJECT_DIR)/build/sysroot
export INITRD_DIR       := $(PROJECT_DIR)/build/initrd
export TOOLCHAIN        := $(PROJECT_DIR)/build/toolchain
export CC               := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-gcc
export CXX              := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-g++
export AS               := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-as
export LD               := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-ld
export OBJCOPY          := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-objcopy
export READELF          := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-readelf
export STRIP            := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-strip
export AR               := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-ar
export RANLIB           := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-ranlib

export UNAME            := $(shell uname)

export ARCH             := x86_64

MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-builtin-variables

MEMORY              = 128

QEMU_UEFI_BIOS      = -bios utils/ovmf-x64/OVMF-pure-efi.fd
QEMU_UEFI_DISKIMG   = -drive format=raw,file=build/disk.img
QEMU_BIOS_DISKIMG   = -drive format=raw,file=build/disk-bios.img

QEMU_CPU_CONFIG     = -smp 4 -cpu qemu64,fsgsbase=true
QEMU_FLAGS          = $(QEMU_CPU_CONFIG) -m $(MEMORY) -nodefaults -no-shutdown -no-reboot -vga std # -d exception,cpu_reset,int

QEMU_E9_PORT_STDIO  = -chardev stdio,id=qemu-debug-out -device isa-debugcon,chardev=qemu-debug-out
QEMU_E9_PORT_FILE   = -chardev file,id=qemu-debug-out,path=build/serialout.log -device isa-debugcon,chardev=qemu-debug-out

INITRD              = $(SYSROOT)/boot/initrd.tar

# apparently osx echo does not do -e
UNAME_IDENT     := $(shell uname)
ifeq ("$(UNAME_IDENT)","Darwin")
	ECHO_CMD    := "echo"
else
	ECHO_CMD    := "echo -e"
endif

BOCHS       ?= bochs
QEMU        ?= qemu-system-x86_64
VIRTUALBOX  ?= VirtualBoxVM


.DEFAULT_GOAL = all


.PHONY: all clean build diskimage qemu debug clean-kernel exportheaders

all: qemu

run-only:
	# starting qemu
	@$(QEMU) $(QEMU_UEFI_BIOS) $(QEMU_UEFI_DISKIMG) $(QEMU_FLAGS) $(QEMU_E9_PORT_STDIO)

debug: diskimage
	# starting qemu
	@$(QEMU) $(QEMU_UEFI_BIOS) $(QEMU_UEFI_DISKIMG) $(QEMU_FLAGS) $(QEMU_E9_PORT_FILE) -s -S -monitor stdio

qemu: diskimage
	# starting qemu
	@$(QEMU) $(QEMU_UEFI_BIOS) $(QEMU_UEFI_DISKIMG) $(QEMU_FLAGS) $(QEMU_E9_PORT_STDIO)

qemu-monitor: diskimage
	# starting qemu
	@$(QEMU) $(QEMU_UEFI_BIOS) $(QEMU_UEFI_DISKIMG) $(QEMU_FLAGS) $(QEMU_E9_PORT_FILE) -monitor stdio

bios: diskimage
	# starting qemu (bios)
	@$(QEMU) $(QEMU_FLAGS) $(QEMU_BIOS_DISKIMG) -serial stdio

bios-debug: diskimage
	# starting qemu (bios)
	@$(QEMU) $(QEMU_FLAGS) $(QEMU_BIOS_DISKIMG) $(QEMU_E9_PORT_FILE) -s -S -monitor stdio

vbox-debug: diskimage
	# starting virtualbox (debug)
	@$(VIRTUALBOX) --startvm "nx" --debug

vbox: diskimage
	# starting virtualbox
	@$(VIRTUALBOX) --startvm "nx"

bochs: diskimage
	# starting bochs
	@$(BOCHS) -qf utils/bochsrc.bxrc




diskimage: build
	@utils/tools/update-diskimage.sh

exportheaders:
	-@rm -r $(SYSROOT)/usr/include/*
	@mkdir -p $(SYSROOT)/usr/include/nx/
	@mkdir -p $(SYSROOT)/usr/include/svr/
	@cp -r libs/libc/include/* $(SYSROOT)/usr/include/
	@cp -r libs/libm/include/* $(SYSROOT)/usr/include/
	@cp -r libs/libnxsc/include/* $(SYSROOT)/usr/include/
	@cp -r kernel/include/export/* $(SYSROOT)/usr/include/nx/
	@cp -r services/*/include/* $(SYSROOT)/usr/include/svr/

compile: exportheaders
	@mkdir -p $(INITRD_DIR)/sys
	@mkdir -p $(INITRD_DIR)/boot
	@mkdir -p $(INITRD_DIR)/drivers
	@mkdir -p $(INITRD_DIR)/services
	@$(MAKE) -s -C libs/libc
	@$(MAKE) -s -C libs/libm
	@$(MAKE) -s -C libs/libkrt
	@$(MAKE) -s -C libs/libnxsc
	@$(MAKE) -s -C libs/tinflate
	@$(MAKE) -s -C efx
	@$(MAKE) -s -C bfx
	@$(MAKE) -s -C kernel
	@$(MAKE) -s -C drivers
	@$(MAKE) -s -C services

make_initrd: compile
	@cp $(SYSROOT)/boot/bfxloader   $(INITRD_DIR)/boot/
	@cp $(SYSROOT)/boot/nxkernel64  $(INITRD_DIR)/boot/
	@cp utils/bootboot_config       $(INITRD_DIR)/sys/config
	@cp utils/bfx_kernel_params     $(INITRD_DIR)/boot/
	@cd $(INITRD_DIR); tar -cf $(INITRD) *
	@gzip -cf9 $(INITRD) > $(INITRD).gz

build: make_initrd

clean:
	@find "efx" -name "*.o" -delete
	@find "libs" -name "*.o" -delete
	@find "bfx" -name "*.o" -delete
	@find "kernel" -name "*.o" -delete

	@find "efx" -name "*.cpp.d" -delete
	@find "libs" -name "*.cpp.d" -delete
	@find "bfx" -name "*.cpp.d" -delete
	@find "kernel" -name "*.cpp.d" -delete

	@rm -f $(SYSROOT)/boot/efxloader
	@rm -f $(SYSROOT)/boot/bfxloader
	@rm -f $(SYSROOT)/boot/nxkernel64
	@rm -f $(SYSROOT)/usr/lib/*.a
	@rm -rf $(SYSROOT)/usr/include/*

clean-kernel:
	@find "kernel" -name "*.o" -delete
	@find "kernel" -name "*.cpp.d" -delete

	@rm -f $(SYSROOT)/boot/nxkernel64







