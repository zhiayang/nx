#!/bin/bash

# replace with where you cloned this
export PROJECT_DIR=/mnt/d/Projects/nx

# don't change this, the rest of the OS build depends on this.
# if you like, you can move it and symlink it.
export PREFIX=$PROJECT_DIR/build/toolchain

# please don't change this
export SYSROOT=$PROJECT_DIR/build/sysroot

# especially not this
export TARGET=x86_64-orionx

export BINUTILS_VERSION="2.31.1"
export GCC_VERSION="8.2.0"

function main() {
	if [ ! -f "$PROJECT_DIR/utils/patches/binutils-$BINUTILS_VERSION.patch" ]; then
		echo "binutils-$BINUTILS_VERSION is not supported! see utils/patches/ for a list of supported versions"
		exit
	fi

	if [ ! -f "$PROJECT_DIR/utils/patches/gcc-$GCC_VERSION.patch" ]; then
		echo "gcc-$GCC_VERSION is not supported! see utils/patches/ for a list of supported versions"
		exit
	fi

	if [ ! $(command -v pv) ]; then
		echo "'pv' is used to display progress, please install it!"
		exit
	fi


	if [ "$1" == "clean" ]; then
		# ! ACHTUNG !
		# rm -rf with impunity is dangerous
		if [ "$PROJECT_DIR" != "" ]; then
			rm -rf $SYSROOT
			rm -rf $PREFIX
			rm -rf $PROJECT_DIR/build
			rm -rf $PROJECT_DIR/toolchain-setup
		fi
		exit
	fi



	# setup colour codes
	_BOLD=`tput bold`
	_NORMAL=`tput sgr0`
	_RED=`tput setaf 1`
	_BLUE=`tput setaf 4`
	_GREEN=`tput setaf 2`


	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}setting up sysroot${_NORMAL}"
	mkdir -p $SYSROOT/usr/include/
	mkdir -p $SYSROOT/usr/lib


	echo "${_BOLD}${_GREEN}=> ${_NORMAL}prefix:  ${_BOLD}$PREFIX${_NORMAL}"
	echo "${_BOLD}${_GREEN}=> ${_NORMAL}sysroot: ${_BOLD}$SYSROOT${_NORMAL}"
	echo ""

	cp -r libs/libc/include/* $SYSROOT/usr/include/

	if [ "$1" != "--skip-toolchain" ]; then
		export SETUP_DIR=$PROJECT_DIR/toolchain-setup
		build_toolchain $SETUP_DIR
	fi


	# we need to build our libc with the cross compiler.
	export CC=$PROJECT_DIR/build/toolchain/bin/x86_64-orionx-gcc
	export CXX=$PROJECT_DIR/build/toolchain/bin/x86_64-orionx-g++
	export LD=$PROJECT_DIR/build/toolchain/bin/x86_64-orionx-gcc
	export CPP=$PROJECT_DIR/build/toolchain/bin/x86_64-orionx-cpp
	export AR=$PROJECT_DIR/build/toolchain/bin/x86_64-orionx-ar
	export AS=$PROJECT_DIR/build/toolchain/bin/x86_64-orionx-as

	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}building libm${_NORMAL}"
	make -C libs/libm install > /dev/null

	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}building libc${_NORMAL}"
	make -C libs/libc > /dev/null

	unset CC
	unset CXX
	unset LD
	unset CPP
	unset AR
	unset AS


	if [ "$1" != "--skip-toolchain" ]; then
		export SETUP_DIR=$PROJECT_DIR/toolchain-setup
		export PATH="$PREFIX/bin:$PATH"

		mkdir -p $SETUP_DIR
		pushd $SETUP_DIR > /dev/null
			# build_libstdcpp || { echo "${_BOLD}${_RED}=> ${_NORMAL}${_BOLD}libstdc++ compilation failed!${_NORMAL}"; exit 1; }
			# echo ""
		popd > /dev/null
	fi


	echo "${_BOLD}${_GREEN}=> ${_NORMAL}${_BOLD}done!${_NORMAL}"
}



function build_toolchain() {
	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}building orionx toolchain${_NORMAL}"
	echo "${_BOLD}${_GREEN}=> ${_NORMAL}${_BOLD}staging directory: $1${_NORMAL}"
	echo ""

	mkdir -p $1
	pushd $1 > /dev/null
		build_binutils || { echo "${_BOLD}${_RED}=> ${_NORMAL}${_BOLD}binutils compilation failed!${_NORMAL}"; exit 1; }
		echo ""

		export PATH="$PREFIX/bin:$PATH"
		build_gcc || { echo "${_BOLD}${_RED}=> ${_NORMAL}${_BOLD}gcc compilation failed!${_NORMAL}"; exit 1; }
	popd > /dev/null
}


function build_binutils() {
	echo "${_BOLD}${_GREEN}=> ${_NORMAL}binutils version: ${_BOLD}$BINUTILS_VERSION${_NORMAL}"
	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}downloading binutils${_NORMAL}"

	if [ ! -f "binutils-$BINUTILS_VERSION.tar.gz" ]; then
		wget -q --show-progress "http://ftpmirror.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.gz"
	fi

	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}extracting binutils${_NORMAL}"

	if [ ! -d "binutils-$BINUTILS_VERSION" ]; then
		tar xf binutils-$BINUTILS_VERSION.tar.gz --checkpoint=.500
		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}patching binutils${_NORMAL}"
		pushd binutils-$BINUTILS_VERSION > /dev/null
			patch -p1 < $PROJECT_DIR/utils/patches/binutils-$BINUTILS_VERSION.patch
		popd > /dev/null
	fi

	echo ""

	if [ -d build-binutils ]; then
		rm -rf build-binutils
	fi

	mkdir -p build-binutils
	pushd build-binutils > /dev/null
		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}configure${_NORMAL}"
		if [ ! $(../binutils-$BINUTILS_VERSION/configure --target=$TARGET --prefix=$PREFIX --with-sysroot=$SYSROOT --disable-nls --disable-werror \
			| pv -t -i 0.5 --name "elapsed time (configure)" > /dev/null) ]; then return 1; fi

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}make${_NORMAL}"
		if [ ! $(make -j all | pv -t -i 0.5 --name "elapsed time (make)" > /dev/null) ]; then return 1; fi

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}install${_NORMAL}"
		if [ ! $(make install | pv -t -i 0.5 --name "elapsed time (install)" > /dev/null) ]; then return 1; fi

		echo "${_BOLD}${_GREEN}=> ${_NORMAL}${_BOLD}done!${_NORMAL}"
	popd > /dev/null
}


function build_gcc() {
	echo "${_BOLD}${_GREEN}=> ${_NORMAL}gcc version: ${_BOLD}$GCC_VERSION${_NORMAL}"
	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}downloading gcc${_NORMAL}"

	if [ ! -f "gcc-$GCC_VERSION.tar.gz" ]; then
		wget -q --show-progress "http://ftpmirror.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.gz"
	fi

	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}extracting gcc${_NORMAL}"

	if [ ! -d "gcc-$GCC_VERSION" ]; then
		tar xf gcc-$GCC_VERSION.tar.gz --checkpoint=.500
		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}patching gcc${_NORMAL}"
		pushd gcc-$GCC_VERSION > /dev/null
			patch -p1 < $PROJECT_DIR/utils/patches/gcc-$GCC_VERSION.patch
		popd > /dev/null
	fi

	echo ""

	if [ -d build-gcc ]; then
		rm -rf build-gcc
	fi

	mkdir -p build-gcc
	pushd build-gcc > /dev/null
		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}configure${_NORMAL}"
		if [ ! $(../gcc-$GCC_VERSION/configure --target=$TARGET --prefix=$PREFIX --with-sysroot=$SYSROOT --disable-nls --disable-werror --disable-libssp --enable-languages=c,c++ | pv -t -i 0.5 --name "elapsed time (configure)" > /dev/null) ]; then return 1; fi

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}make (gcc)${_NORMAL}"
		if [ ! $(make -j all-gcc | pv -t -i 0.5 --name "elapsed time (gcc)"  > /dev/null) ]; then return 1; fi

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}make (libgcc)${_NORMAL}"
		if [ ! $(make -j all-target-libgcc | pv -t -i 0.5 --name "elapsed time (libgcc)"  > /dev/null) ]; then return 1; fi

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}install${_NORMAL}"
		if [ ! $(make install-gcc install-target-libgcc | pv -t -i 0.5 --name "elapsed time (install)"  > /dev/null) ]; then return 1; fi

		echo "${_BOLD}${_GREEN}=> ${_NORMAL}${_BOLD}done!${_NORMAL}"
	popd > /dev/null
}



function build_libstdcpp() {

	pushd build-gcc > /dev/null
		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}make (libstdc++)${_NORMAL}"
		if [ ! $(make -j all-target-libstdc++-v3  | pv -t -i 0.5 --name "elapsed time (libstdc++)" > /dev/null) ]; then return 1; fi

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}install${_NORMAL}"
		if [ ! $(make install-target-libstdc++-v3 | pv -t -i 0.5 --name "elapsed time (install)" > /dev/null) ]; then return 1; fi

		echo "${_BOLD}${_GREEN}=> ${_NORMAL}${_BOLD}done!${_NORMAL}"
	popd > /dev/null
}















main "$@"




