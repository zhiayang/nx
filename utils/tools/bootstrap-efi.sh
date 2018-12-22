#!/usr/bin/env bash

# setup colour codes
_BOLD=`tput bold`
_NORMAL=`tput sgr0`
_RED=`tput setaf 1`
_BLUE=`tput setaf 4`
_GREEN=`tput setaf 2`

if [[ $PROJECT_DIR == "" || $SETUP_DIR == "" ]]; then
	echo "please set the project directory! (or invoke using './bootstrap --skip-toolchain --skip-libc --skip-sysroot' to only run this script"
	exit 1
fi

export EFI_PREFIX=$PROJECT_DIR/build/efi/toolchain
export EFI_SYSROOT=$PROJECT_DIR/build/efi/sysroot
export EFI_TARGET=x86_64-w64-mingw32

echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}building efi toolchain${_NORMAL}"
echo "${_BOLD}${_GREEN}=> ${_NORMAL}${_BOLD}staging directory: $SETUP_DIR${_NORMAL}"
echo ""

mkdir -p $PROJECT_DIR/build/efi/sysroot
mkdir -p $PROJECT_DIR/build/efi/toolchain

pushd $SETUP_DIR > /dev/null

	echo "${_BOLD}${_GREEN}=> ${_NORMAL}binutils version: ${_BOLD}$BINUTILS_VERSION${_NORMAL}"
	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}downloading binutils${_NORMAL}"

	if [ ! -f "binutils-$BINUTILS_VERSION.tar.gz" ]; then
		wget -q --show-progress "http://ftpmirror.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.gz"
	fi

	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}extracting binutils${_NORMAL}"

	if [ ! -d "binutils-$BINUTILS_VERSION" ]; then
		tar xf binutils-$BINUTILS_VERSION.tar.gz --checkpoint=.250
		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}patching binutils${_NORMAL}"
		echo ""
		pushd binutils-$BINUTILS_VERSION > /dev/null
			patch -p1 < $PROJECT_DIR/utils/patches/binutils-$BINUTILS_VERSION.patch
		popd > /dev/null
	fi

	echo ""

	if [ -d build-efi-binutils ]; then
		rm -rf build-efi-binutils
	fi

	mkdir -p build-efi-binutils
	pushd build-efi-binutils > /dev/null
		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}configure${_NORMAL}"
		../binutils-$BINUTILS_VERSION/configure --target=$EFI_TARGET --prefix=$EFI_PREFIX --with-sysroot=$EFI_SYSROOT --disable-nls --disable-werror 2>1 | pv -t -i 0.5 --name 'elapsed' > binutils-configure.log || { exit 1; }

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}make${_NORMAL}"
		make -j$NUM_MAKE_JOBS all 2>1 | pv -t -i 0.5 --name 'elapsed' > binutils-make.log || { exit 1; }

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}install${_NORMAL}"
		make install 2>1 | pv -t -i 0.5 --name 'elapsed' > binutils-install.log || { exit 1; }

		echo "${_BOLD}${_GREEN}=> ${_NORMAL}${_BOLD}done!${_NORMAL}"
	popd > /dev/null










	echo "${_BOLD}${_GREEN}=> ${_NORMAL}gcc version: ${_BOLD}$GCC_VERSION${_NORMAL}"
	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}downloading gcc${_NORMAL}"

	if [ ! -f "gcc-$GCC_VERSION.tar.gz" ]; then
		wget -q --show-progress "http://ftpmirror.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.gz"
	fi

	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}extracting gcc${_NORMAL}"

	if [ ! -d "gcc-$GCC_VERSION" ]; then
		tar xf gcc-$GCC_VERSION.tar.gz --checkpoint=.250
		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}patching gcc${_NORMAL}"
		echo ""
		pushd gcc-$GCC_VERSION > /dev/null
			patch -p1 < $PROJECT_DIR/utils/patches/gcc-$GCC_VERSION.patch
		popd > /dev/null
	fi

	echo ""

	if [ -d build-efi-gcc ]; then
		rm -rf build-efi-gcc
	fi

	mkdir -p build-efi-gcc
	pushd build-efi-gcc > /dev/null
		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}configure${_NORMAL}"
		../gcc-$GCC_VERSION/configure --target=$EFI_TARGET --prefix=$EFI_PREFIX --with-sysroot=$EFI_SYSROOT --disable-nls --disable-werror --enable-languages=c,c++ --without-headers 2>1 | pv -t -i 0.5 --name 'elapsed' > gcc-configure.log || { exit 1; }

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}make (gcc)${_NORMAL}"
		make -j$NUM_MAKE_JOBS all-gcc 2>1 | pv -t -i 0.5 --name 'elapsed' > gcc-make.log || { exit 1; }

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}make (libgcc)${_NORMAL}"
		make -j$NUM_MAKE_JOBS all-target-libgcc 2>1 | pv -t -i 0.5 --name 'elapsed' > gcc-make.log || { exit 1; }

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}install${_NORMAL}"
		make install-gcc install-target-libgcc 2>1 | pv -t -i 0.5 --name 'elapsed' > gcc-install.log || { exit 1; }

		echo "${_BOLD}${_GREEN}=> ${_NORMAL}${_BOLD}done!${_NORMAL}"
	popd > /dev/null



popd > /dev/null














