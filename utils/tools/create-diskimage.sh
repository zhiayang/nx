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


if [ -z "$PROJECT_DIR" ] || [ -z "$PREFIX" ]; then
	echo "please set the project directory! (or invoke using './bootstrap --skip-toolchain --skip-libc --skip-sysroot' to only run this script"
	exit 1
fi


# check for requisite tools
if [ $(uname) == "Darwin" ]; then
	# on osx we need to use gdisk. homebrew has it as gptfdisk
	# also, we come with newfs_msdos, so we don't need mkfs.vfat.
	if [ ! $(command -v gdisk) ]; then
		echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}'gdisk' not found${_NORMAL} (gptfdisk)"
		exit 1
	fi
else
	if [ ! $(command -v fdisk) ]; then
		echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}'fdisk' not found${_NORMAL} (fdisk)"
		exit 1
	fi

	if [ ! $(command -v mkfs.vfat) ]; then
		echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}'mkfs.vfat' not found${_NORMAL} (dosfstools)"
		exit 1
	fi
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

DD_STATUS="status=none"
if [ $(uname) == "Darwin" ]; then
	DD_STATUS=""
fi


echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}creating disk image${_NORMAL}"

mkdir -p $PROJECT_DIR/build
pushd $PROJECT_DIR/build > /dev/null

	if [ -e $UEFI_OUTPUT_DISK ]; then rm $UEFI_OUTPUT_DISK; fi

	echo "${_BOLD}${_GREEN}=> ${_NORMAL}${_BOLD}image size: $(expr $SECTOR_COUNT \* $SECTOR_SIZE / 1024 / 1024) mb${_NORMAL}"
	echo ""

	dd if=/dev/zero of=$UEFI_OUTPUT_DISK bs=$SECTOR_SIZE count=$SECTOR_COUNT $DD_STATUS


	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}downloading rEFInd${_NORMAL}"
	if [ ! -d $PROJECT_DIR/utils/refind-bin-$REFIND_VERSION ]; then
		wget -q --show-progress https://downloads.sourceforge.net/project/refind/$REFIND_VERSION/refind-bin-$REFIND_VERSION.zip
		unzip -qq -d $PROJECT_DIR/utils refind-bin-$REFIND_VERSION.zip

		rm refind-bin-$REFIND_VERSION.zip
	fi

	echo ""

	# make temporary disk images for our esp image and our data image, which we will later `dd` into the disk.img at the appropriate offsets.

	if [ -e esp-part.img ]; then rm esp-part.img; fi
	if [ -e root-part.img ]; then rm root-part.img; fi

	dd if=/dev/zero of=esp-part.img bs=$SECTOR_SIZE count=$(expr $ESP_PARTITION_END - $ESP_PARTITION_START + 1) $DD_STATUS
	dd if=/dev/zero of=root-part.img bs=$SECTOR_SIZE count=$(expr $ROOT_PARTITION_END - $ROOT_PARTITION_START + 1) $DD_STATUS

	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}creating GPT${_NORMAL}"
	if [ $(uname) == "Darwin" ]; then
		# a transcript:
		# 1. o                    - make a new guid partition table
		# 2. y                    - confirm
		# 3. n                    - make a new partition
		# 4. 1                    - partition numero uno
		# 5. ESP_PARTITION_START  - partition start
		# 6. ESP_PARTITION_END    - partition end
		# 7. ef00                 - EFI system
		# 8. n                    - make a new partition
		# 9. 2                    - partition numero dos
		# 10. ROOT_PARTITION_START - partition start
		# 11. ROOT_PARTITION_END  - partition end
		# 12. 0700                - microsoft basic data (fat32)
		# 13. x                   - expert mode
		# 14. c                   - change partition uuid
		# 15. 1                   - select partition 1
		# 16. ESP_PART_UUID       - uuid
		# 17. c                   - change partition uuid
		# 18. 2                   - select partition 2
		# 19. ROOT_PART_UUID      - uuid
		# 20. w                   - write changes and quit
		# 21. y                   - confirm


		echo -e "o\ny\nn\n1\n$ESP_PARTITION_START\n$ESP_PARTITION_END\nef00\nn\n2\n$ROOT_PARTITION_START\n$ROOT_PARTITION_END\n0700\nx\nc\n1\n$ESP_PART_UUID\nc\n2\n$ROOT_PART_UUID\nw\ny\n" | gdisk $UEFI_OUTPUT_DISK > /dev/null

		ESP_IDENT=$(hdiutil attach -nomount esp-part.img)
		newfs_msdos -F 16 -v "EFIPART" $ESP_IDENT > /dev/null

		ROOT_IDENT=$(hdiutil attach -nomount root-part.img)
		newfs_msdos -F 32 -v "NX" $ROOT_IDENT > /dev/null

		hdiutil detach $ESP_IDENT
		hdiutil detach $ROOT_IDENT

	elif [ $(uname) == "Linux" ]; then

		# make the filesystems -- this is os-specific so we drop back into the if-statement for a while

		# a transcript:
		# 1. g                    - make a new guid partition table
		# 2. n                    - make a new partition
		# 3. 1                    - partition numero uno
		# 4. ESP_PARTITION_START  - partition start
		# 5. ESP_PARTITION_END    - partition end
		# 6. n                    - make a new partition
		# 7. 2                    - partition numero dos
		# 8. ROOT_PARTITION_START - partition start
		# 9. ROOT_PARTITION_END   - partition end
		# 10. t                   - change partition type
		# 11. 1                   - select partition 1
		# 12. 01                  - EFI system
		# 13. t                   - change partition type
		# 14. 2                   - select partition 2
		# 15. 11                  - microsoft basic data (fat32)
		# 16. x                   - expert mode
		# 17. u                   - change partition uuid
		# 18. 1                   - select partition 1
		# 19. ESP_PART_UUID       - uuid
		# 20. u                   - change partition uuid
		# 21. 2                   - select partition 2
		# 22. ROOT_PART_UUID      - uuid
		# 23. r                   - return to non-expert menu
		# 24. w                   - write changes and quit

		echo -e "g\nn\n1\n$ESP_PARTITION_START\n$ESP_PARTITION_END\nn\n2\n$ROOT_PARTITION_START\n$ROOT_PARTITION_END\nt\n1\n01\nt\n2\n11\nx\nu\n1\n\
$ESP_PART_UUID\nu\n2\n$ROOT_PART_UUID\nr\nw\n" | fdisk $UEFI_OUTPUT_DISK > /dev/null

		mkfs.vfat -n "EFIPART" -F 16 esp-part.img > /dev/null
		mkfs.vfat -n "NX" -F 32 root-part.img > /dev/null

	else
		echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}$(uname) is not a supported build platform!${_NORMAL}"
		exit 1
	fi


	export MTOOLS_SKIP_CHECK=1

	# note that we create one image for UEFI, and one image for BIOS.
	cp esp-part.img bios-part.img
	cp $UEFI_OUTPUT_DISK $BIOS_OUTPUT_DISK

	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}creating uefi boot partition${_NORMAL}"

	# # this is the UEFI part:
	mmd -D O -i esp-part.img ::/EFI
	mmd -D O -i esp-part.img ::/EFI/BOOT
	mmd -D O -i esp-part.img ::/EFI/BOOT/refind

	mcopy -snQ -i esp-part.img $PROJECT_DIR/utils/refind-bin-$REFIND_VERSION/refind/drivers_x64 ::/EFI/BOOT/refind/
	mcopy -snQ -i esp-part.img $PROJECT_DIR/utils/refind-bin-$REFIND_VERSION/refind/tools_x64 ::/EFI/BOOT/refind/
	mcopy -snQ -i esp-part.img $PROJECT_DIR/utils/refind-bin-$REFIND_VERSION/refind/refind_x64.efi ::/EFI/BOOT/bootx64.efi

	dd if=esp-part.img bs=$SECTOR_SIZE seek=$ESP_PARTITION_START of=$UEFI_OUTPUT_DISK \
		count=$(expr $ESP_PARTITION_END - $ESP_PARTITION_START + 1) conv=notrunc $DD_STATUS

	dd if=root-part.img bs=$SECTOR_SIZE seek=$ROOT_PARTITION_START of=$UEFI_OUTPUT_DISK \
		count=$(expr $ROOT_PARTITION_END - $ROOT_PARTITION_START + 1) conv=notrunc $DD_STATUS


	# this is the BIOS part:
	# first download & compile mkboot
	if [ ! -e "bootboot/mkboot" ]; then
		mkdir -p bootboot
		pushd bootboot > /dev/null

		if [ ! $(command -v xxd) ]; then
			echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}'xxd' not found${_NORMAL}"
			exit 1
		fi

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}downloading bootboot${_NORMAL}" \
				"(https://gitlab.com/bztsrc/bootboot)"
		wget -qnc https://gitlab.com/bztsrc/bootboot/raw/master/boot.bin
		wget -qnc https://gitlab.com/bztsrc/bootboot/raw/master/x86_64-bios/mkboot.c

		xxd -i boot.bin | sed 's/boot_bin/_binary____boot_bin_start/g' > boot_include.c
		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}compiling mkboot${_NORMAL}"

		cc -o mkboot mkboot.c boot_include.c

		popd > /dev/null
	fi

	# note that the BIOS version uses uncompressed initrd, since the gzip decompression
	# takes too damn long for some reason.
	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}creating bios boot partition${_NORMAL}"
	mmd -D O -i bios-part.img ::/BOOTBOOT
	mcopy -snQ -i bios-part.img $PROJECT_DIR/utils/bootboot.bin ::/BOOTBOOT/LOADER
	mcopy -snQ -i bios-part.img sysroot/boot/initrd.tar         ::/BOOTBOOT/INITRD

	# we can reuse the same geometry numbers, but start at 0.
	dd if=bios-part.img bs=$SECTOR_SIZE seek=$ESP_PARTITION_START of=$BIOS_OUTPUT_DISK \
		count=$(expr $ESP_PARTITION_END - $ESP_PARTITION_START + 1) conv=notrunc $DD_STATUS

	dd if=root-part.img bs=$SECTOR_SIZE seek=$ROOT_PARTITION_START of=$BIOS_OUTPUT_DISK \
		count=$(expr $ROOT_PARTITION_END - $ROOT_PARTITION_START + 1) conv=notrunc $DD_STATUS

	bootboot/mkboot disk-bios.img

	echo "${_BOLD}${_GREEN}=> ${_NORMAL}${_BOLD}done!${_NORMAL}"
	$PROJECT_DIR/utils/tools/update-diskimage.sh

popd > /dev/null




















