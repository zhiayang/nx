#!/usr/bin/env bash

# setup colour codes
_BOLD=`tput bold`
_NORMAL=`tput sgr0`
_RED=`tput setaf 1`
_BLUE=`tput setaf 4`
_GREEN=`tput setaf 2`

REFIND_VERSION=0.11.4

UEFI_OUTPUT_DISK=disk.img
BIOS_OUTPUT_DISK=disk-bios.img


if [ $PROJECT_DIR == "" ]; then
	echo "please set the project directory! (or invoke using './bootstrap --skip-toolchain --skip-libc --skip-sysroot' to only run this script"
	exit 1
fi

if [ ! $(command -v mcopy) ]; then
	echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}'mcopy' not found${_NORMAL} (mtools)"
	exit 1
fi

source $PROJECT_DIR/utils/tools/disk-geometry.sh


echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}updating disk image${_NORMAL}"

mkdir -p $PROJECT_DIR/build
pushd $PROJECT_DIR/build > /dev/null

	if [ ! -e $UEFI_OUTPUT_DISK ]; then
		echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}disk image not found -- please run ./bootstrap.sh${_NORMAL}"
		exit 1
	fi

	export MTOOLS_SKIP_CHECK=1
	ESP_IMG_OFFSET=$UEFI_OUTPUT_DISK@@$(expr $ESP_PARTITION_START \* $SECTOR_SIZE)
	ROOT_IMG_OFFSET=$UEFI_OUTPUT_DISK@@$(expr $ROOT_PARTITION_START \* $SECTOR_SIZE)

	BIOS_ESP_IMG_OFFSET=$BIOS_OUTPUT_DISK@@$(expr $ESP_PARTITION_START \* $SECTOR_SIZE)
	BIOS_ROOT_IMG_OFFSET=$BIOS_OUTPUT_DISK@@$(expr $ROOT_PARTITION_START \* $SECTOR_SIZE)

	mcopy -D o -snQ -i $ESP_IMG_OFFSET $PROJECT_DIR/utils/refind.conf ::/EFI/BOOT/refind.conf

	mcopy -D o -snQ -i $BIOS_ESP_IMG_OFFSET $SYSROOT/boot/initrd.tar ::/BOOTBOOT/INITRD

	mdeltree -i $ROOT_IMG_OFFSET ::/*
	mcopy -D o -snQ -i $ROOT_IMG_OFFSET $PROJECT_DIR/build/sysroot/* ::/

	mdeltree -i $BIOS_ROOT_IMG_OFFSET ::/*
	mcopy -D o -snQ -i $BIOS_ROOT_IMG_OFFSET $PROJECT_DIR/build/sysroot/* ::/

popd > /dev/null

echo "${_BOLD}${_GREEN}=> ${_NORMAL}${_BOLD}done!${_NORMAL}"



















