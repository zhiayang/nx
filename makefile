# Makefile for Orion-X3/Orion-X4/mx and derivatives
# Written in 2011
# This makefile is licensed under the WTFPL

export ARCH                 := x86_64
export PROJECT_DIR          := $(shell pwd)
export UNAME                := $(shell uname)

export SYSROOT              := $(PROJECT_DIR)/build/sysroot
export INITRD_DIR           := $(PROJECT_DIR)/build/initrd
export TOOLCHAIN            := $(PROJECT_DIR)/build/toolchain
export CC                   := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-gcc
export CXX                  := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-g++
export AS                   := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-as
export LD                   := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-ld
export OBJCOPY              := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-objcopy
export READELF              := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-readelf
export STRIP                := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-strip
export AR                   := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-ar
export RANLIB               := $(PROJECT_DIR)/build/toolchain/bin/x86_64-orionx-ranlib

export INITRD               := $(SYSROOT)/boot/initrd.tar

# used by kernel, libkrt, and libtinflate
export KERNEL_SSE_DISABLERS := -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-80387 -mno-fp-ret-in-387



MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-builtin-variables

MEMORY                  := 128

QEMU_UEFI_BIOS          := -bios utils/ovmf-x64/OVMF-pure-efi.fd
QEMU_UEFI_DISKIMG       := -drive format=raw,file=build/disk.img
QEMU_BIOS_DISKIMG       := -drive format=raw,file=build/disk-bios.img

QEMU_CPU_CONFIG         := -smp 4 -cpu qemu64,fsgsbase=true,+xsave,+xsaveopt,+smep,+smap
QEMU_FLAGS              := $(QEMU_CPU_CONFIG) -m $(MEMORY) -nodefaults -no-shutdown -no-reboot -vga std # -d exception,cpu_reset,int

QEMU_SERIAL             := -chardev stdio,id=char0,logfile=build/serialout.log -serial chardev:char0

QEMU_E9_PORT_STDIO      := -chardev stdio,id=qemu-debug-out -device isa-debugcon,chardev=qemu-debug-out
QEMU_E9_PORT_FILE       := -chardev file,id=qemu-debug-out,path=build/serialout.log -device isa-debugcon,chardev=qemu-debug-out


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


.PHONY: all clean build diskimage qemu debug clean-kernel export-headers make-initrd

all: qemu

run-only:
	# starting qemu
	@$(QEMU) $(QEMU_UEFI_BIOS) $(QEMU_UEFI_DISKIMG) $(QEMU_FLAGS) $(QEMU_E9_PORT_STDIO)

debug: diskimage
	# starting qemu
	@$(QEMU) $(QEMU_UEFI_BIOS) $(QEMU_UEFI_DISKIMG) $(QEMU_FLAGS) $(QEMU_E9_PORT_FILE) -monitor stdio -s -S

qemu: diskimage
	# starting qemu
	@$(QEMU) $(QEMU_UEFI_BIOS) $(QEMU_UEFI_DISKIMG) $(QEMU_FLAGS) $(QEMU_SERIAL)

qemu-monitor: diskimage
	# starting qemu
	@$(QEMU) $(QEMU_UEFI_BIOS) $(QEMU_UEFI_DISKIMG) $(QEMU_FLAGS) $(QEMU_E9_PORT_FILE) -monitor stdio

bios: diskimage
	# starting qemu (bios)
	@$(QEMU) $(QEMU_FLAGS) $(QEMU_BIOS_DISKIMG) $(QEMU_E9_PORT_STDIO)

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

make-folders:
	@mkdir -p $(SYSROOT)/usr/include/nx/
	@mkdir -p $(SYSROOT)/usr/include/svr/
	@mkdir -p $(INITRD_DIR)/sys
	@mkdir -p $(INITRD_DIR)/boot
	@mkdir -p $(INITRD_DIR)/drivers
	@mkdir -p $(INITRD_DIR)/services

export-headers: $(shell find libs/libc/include -type f) $(shell find libs/libm/include -type f) $(shell find libs/libnxsc/include -type f) $(shell find kernel/include/export -type f) $(shell find services/*/include -type f) | make-folders
	-@find $(SYSROOT)/usr/include/ -type f | xargs rm
	@cp -R libs/libc/include/*                  $(SYSROOT)/usr/include/
	@cp -R libs/libm/include/*                  $(SYSROOT)/usr/include/
	@cp -R libs/libnxsc/include/*               $(SYSROOT)/usr/include/
	@cp -R kernel/include/export/*              $(SYSROOT)/usr/include/nx/
	@cp -R services/tty-svr/include/export/*    $(SYSROOT)/usr/include/svr/
	@cp -R services/vfs-svr/include/export/*    $(SYSROOT)/usr/include/svr/

$(SYSROOT)/boot/nxkernel64: $(shell find kernel)
	@$(MAKE) -s -C kernel

$(SYSROOT)/boot/bfxloader: $(shell find bfx)
	@$(MAKE) -s -C bfx

compile: export-headers
	@$(MAKE) -s -C libs/libc
	@$(MAKE) -s -C libs/libm
	@$(MAKE) -s -C libs/libkrt
	@$(MAKE) -s -C libs/libnxsc
	@$(MAKE) -s -C libs/miniz
	@$(MAKE) -s -C efx
	@$(MAKE) -s -C bfx
	@$(MAKE) -s -C kernel
	@$(MAKE) -s -C drivers
	@$(MAKE) -s -C services

$(INITRD).gz: $(shell find $(INITRD_DIR) | sed 's/ /?/g') compile
	@utils/tools/update-initrd.sh


make-initrd: $(INITRD).gz

build: make-initrd

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







