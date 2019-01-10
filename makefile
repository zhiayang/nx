# Makefile for Orion-X3/Orion-X4/mx and derivatives
# Written in 2011
# This makefile is licensed under the WTFPL

export PROJECT_DIR      := $(shell pwd)

export SYSROOT          := $(PROJECT_DIR)/build/sysroot
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

MEMORY              = 256

QEMU_UEFI_BIOS      = -bios utils/ovmf-x64/OVMF-pure-efi.fd
QEMU_DISK_IMAGE     = -drive format=raw,file=build/disk.img

QEMU_FLAGS          = -vga std -m $(MEMORY) $(QEMU_UEFI_BIOS) $(QEMU_DISK_IMAGE) -no-shutdown -no-reboot

QEMU_E9_PORT_STDIO  = -chardev stdio,id=qemu-debug-out -device isa-debugcon,chardev=qemu-debug-out
QEMU_E9_PORT_FILE   = -chardev file,id=qemu-debug-out,path=build/serialout.log -device isa-debugcon,chardev=qemu-debug-out


.DEFAULT_GOAL = all

INITRD              = $(SYSROOT)/boot/initrd.tar.gz


.PHONY: all clean build diskimage qemu debug clean-kernel

all: qemu

debug: diskimage
	@echo -e "# starting qemu\n"
	@$(QEMU) $(QEMU_FLAGS) $(QEMU_E9_PORT_FILE) -s -S -monitor stdio

qemu: diskimage
	@echo -e "# starting qemu\n"
	@$(QEMU) $(QEMU_FLAGS) $(QEMU_E9_PORT_STDIO)

vbox: diskimage
	@echo -e "# starting virtualbox\n"
	@/mnt/c/Program\ Files/Oracle/VirtualBox/VBoxManage.exe startvm "nx"



diskimage: build $(INITRD)
	@utils/tools/update-diskimage.sh

build:
	@$(MAKE) -s -C libs/libc
	@$(MAKE) -s -C libs/libm
	@$(MAKE) -s -C libs/libkrt
	@$(MAKE) -s -C libs/tinflate
	@$(MAKE) -s -C efx
	@$(MAKE) -s -C kernel

$(INITRD): $(shell find $(PROJECT_DIR)/build/initrd -name "*")
	@tar -cvf - -C $(PROJECT_DIR)/build/initrd/ . | gzip -9 - > $(INITRD)

clean:
	@find "efx" -name "*.o" -delete
	@find "libs" -name "*.o" -delete
	@find "kernel" -name "*.o" -delete

	@find "efx" -name "*.cpp.d" -delete
	@find "libs" -name "*.cpp.d" -delete
	@find "kernel" -name "*.cpp.d" -delete

	@rm -f $(SYSROOT)/boot/efxloader
	@rm -f $(SYSROOT)/boot/nxkernel64
	@rm -f $(SYSROOT)/usr/lib/*.a

clean-kernel:
	@find "kernel" -name "*.o" -delete
	@find "kernel" -name "*.cpp.d" -delete

	@rm -f $(SYSROOT)/boot/nxkernel64







