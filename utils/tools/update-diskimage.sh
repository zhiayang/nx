#!/usr/bin/env bash

# setup colour codes
_BOLD=`tput bold`
_NORMAL=`tput sgr0`
_RED=`tput setaf 1`
_BLUE=`tput setaf 4`
_GREEN=`tput setaf 2`

REFIND_VERSION=0.11.4

OUTPUT_DISK=disk.img


if [ $PROJECT_DIR == "" ]; then
	echo "please set the project directory! (or invoke using './bootstrap --skip-toolchain --skip-libc --skip-sysroot' to only run this script"
	exit 1
fi

# check for requisite tools
if [ ! $(command -v fdisk) ]; then
	echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}'fdisk' not found${_NORMAL} (fdisk)"
	exit 1
fi

if [ ! $(command -v mkfs.vfat) ]; then
	echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}'mkfs.vfat' not found${_NORMAL} (dosfstools)"
	exit 1
fi

if [ ! $(command -v unzip) ]; then
	echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}'unzip' not found${_NORMAL} (unzip)"
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

	if [ ! -e $OUTPUT_DISK ] || [ ! -e esp-part.img ] || [ ! -e root-part.img ]; then
		echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}disk image not found -- please run ./bootstrap.sh${_NORMAL}"
		exit 1
	fi

	mcopy -D o -snQ -i esp-part.img $PROJECT_DIR/utils/refind.conf ::/EFI/BOOT/refind.conf

	mdeltree -i root-part.img ::/*
	mcopy -D o -snQ -i root-part.img $PROJECT_DIR/build/sysroot/* ::/

	# join the two partitions together.
	dd if=esp-part.img of=disk.img bs=$SECTOR_SIZE count=$(expr $ESP_PARTITION_END - $ESP_PARTITION_START + 1) seek=$ESP_PARTITION_START conv=notrunc status=none
	dd if=root-part.img of=disk.img bs=$SECTOR_SIZE count=$(expr $ROOT_PARTITION_END - $ROOT_PARTITION_START + 1) seek=$ROOT_PARTITION_START conv=notrunc status=none


popd > /dev/null

echo "${_BOLD}${_GREEN}=> ${_NORMAL}${_BOLD}done!${_NORMAL}"



















