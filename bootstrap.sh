#!/usr/bin/env bash

# replace with where you cloned this
export PROJECT_DIR=$(pwd)

# don't change this, the rest of the OS build depends on this.
# if you like, you can move it and symlink it.
export PREFIX=$PROJECT_DIR/build/toolchain

# please don't change this
export SYSROOT=$PROJECT_DIR/build/sysroot

# especially not this
export TARGET=x86_64-orionx

export BINUTILS_VERSION="2.31.1"
export GCC_VERSION="8.2.0"

export NUM_MAKE_JOBS=16

SKIP_SETUP_LIBC=false
SKIP_SETUP_SYSROOT=false
SKIP_SETUP_DISKIMAGE=false
SKIP_SETUP_TOOLCHAIN=false
SKIP_SETUP_EFI_TOOLCHAIN=false
SKIP_SETUP_TOOLCHAIN_BINUTILS=false
SKIP_SETUP_TOOLCHAIN_GCC=false

# setup colour codes
_BOLD=`tput bold`
_NORMAL=`tput sgr0`
_RED=`tput setaf 1`
_BLUE=`tput setaf 4`
_GREEN=`tput setaf 2`

TAR_CHECKPOINT_ARG="--checkpoint=.250"

function main() {

	if [ $(uname) == "Darwin" ]; then
		# the tar on osx doesn't support --checkpoint
		TAR_CHECKPOINT_ARG=""
	fi

	if [ "$1" == "clean" ]; then
		# ! ACHTUNG !
		# rm -rf with impunity is dangerous

		echo "${_BOLD}${_RED}=> ${_NORMAL}${_BOLD}destroying development environment${_NORMAL}"
		echo ""

		if [ "$PROJECT_DIR" != "" ]; then
			rm -rf $SYSROOT
			rm -rf $PREFIX
			rm -rf $PROJECT_DIR/build
			rm -rf $PROJECT_DIR/toolchain-setup
		fi
		exit
	fi


	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}bootstrapping development environment${_NORMAL}"
	echo ""

	chmod +x $PROJECT_DIR/utils/tools/*.sh







	if [ ! -f "$PROJECT_DIR/utils/patches/binutils-$BINUTILS_VERSION.patch" ]; then
		echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}binutils-$BINUTILS_VERSION is not supported!${_NORMAL} see utils/patches/ for a list of supported versions"
		exit 1
	fi

	if [ ! -f "$PROJECT_DIR/utils/patches/gcc-$GCC_VERSION.patch" ]; then
		echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}gcc-$GCC_VERSION is not supported!${_NORMAL} see utils/patches/ for a list of supported versions"
		exit 1
	fi

	if [ ! $(command -v pv) ]; then
		echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}'pv' not found${_NORMAL}"
		exit 1
	fi


	if [ $SKIP_SETUP_SYSROOT == false ]; then
		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}setting up sysroot${_NORMAL}"
		mkdir -p $SYSROOT/usr/include
		mkdir -p $SYSROOT/usr/lib

		mkdir -p $SYSROOT/boot

		mkdir -p $SYSROOT/drivers
		mkdir -p $SYSROOT/services
	fi

	echo "${_BOLD}${_GREEN}=> ${_NORMAL}prefix:  ${_BOLD}$PREFIX${_NORMAL}"
	echo "${_BOLD}${_GREEN}=> ${_NORMAL}sysroot: ${_BOLD}$SYSROOT${_NORMAL}"
	echo ""

	cp -r libs/libc/include/* $SYSROOT/usr/include/

	if [ $SKIP_SETUP_TOOLCHAIN == false ]; then
		export SETUP_DIR=$PROJECT_DIR/toolchain-setup
		build_toolchain $SETUP_DIR
		echo ""
	fi

	if [ $SKIP_SETUP_LIBC == false ]; then
		# we need to build our libc with the cross compiler.
		export CC=$PROJECT_DIR/build/toolchain/bin/x86_64-orionx-gcc
		export CXX=$PROJECT_DIR/build/toolchain/bin/x86_64-orionx-g++
		export LD=$PROJECT_DIR/build/toolchain/bin/x86_64-orionx-gcc
		export CPP=$PROJECT_DIR/build/toolchain/bin/x86_64-orionx-cpp
		export AR=$PROJECT_DIR/build/toolchain/bin/x86_64-orionx-ar
		export AS=$PROJECT_DIR/build/toolchain/bin/x86_64-orionx-as

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}building libm${_NORMAL}"
		make -C libs/libm install > /dev/null || { echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}libm compilation failed!${_NORMAL}"; exit 1; }

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}building libc${_NORMAL}"
		make -C libs/libc > /dev/null || { echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}libc compilation failed!${_NORMAL}"; exit 1; }

		echo ""

		unset CC
		unset CXX
		unset LD
		unset CPP
		unset AR
		unset AS
	fi

	if [ $SKIP_SETUP_TOOLCHAIN == false ]; then
		export SETUP_DIR=$PROJECT_DIR/toolchain-setup
		export PATH="$PREFIX/bin:$PATH"

		mkdir -p $SETUP_DIR
		pushd $SETUP_DIR > /dev/null
			build_libstdcpp || { echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}libstdc++ compilation failed!${_NORMAL}"; exit 1; }
			echo ""
		popd > /dev/null
	fi


	if [ $SKIP_SETUP_DISKIMAGE == false ]; then
		chmod +x $PROJECT_DIR/utils/tools/create-diskimage.sh
		$PROJECT_DIR/utils/tools/create-diskimage.sh || { echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}disk image creation failed!${_NORMAL}"; exit 1; }
		echo ""
	fi


	if [ $SKIP_SETUP_EFI_TOOLCHAIN == false ]; then

		export SETUP_DIR=$PROJECT_DIR/toolchain-setup
		mkdir -p $SETUP_DIR

		chmod +x $PROJECT_DIR/utils/tools/bootstrap-efi.sh
		$PROJECT_DIR/utils/tools/bootstrap-efi.sh || { echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}efi toolchain compilation failed!${_NORMAL}"; exit 1; }
		echo ""
	fi



	mkdir -p $PROJECT_DIR/build/initrd/fonts
	mkdir -p $PROJECT_DIR/build/initrd/drivers
	mkdir -p $PROJECT_DIR/build/initrd/services

	cp $PROJECT_DIR/utils/fonts/deja-vu/DejaVuSansMono.ttf $PROJECT_DIR/build/initrd/fonts/mono.ttf
	cp $PROJECT_DIR/utils/fonts/deja-vu/DejaVuSansMono-Bold.ttf $PROJECT_DIR/build/initrd/fonts/mono-bold.ttf


	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}environment is set up!${_NORMAL}"
}



function build_toolchain() {
	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}building orionx toolchain${_NORMAL}"
	echo "${_BOLD}${_GREEN}=> ${_NORMAL}${_BOLD}staging directory: $1${_NORMAL}"
	echo ""

	mkdir -p $1
	pushd $1 > /dev/null
		build_binutils || { echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}binutils compilation failed!${_NORMAL}"; exit 1; }
		echo ""

		export PATH="$PREFIX/bin:$PATH"
		build_gcc || { echo "${_BOLD}${_RED}!> ${_NORMAL}${_BOLD}gcc compilation failed!${_NORMAL}"; exit 1; }
	popd > /dev/null
}


function build_binutils() {
	if [ $SKIP_SETUP_TOOLCHAIN_BINUTILS == true ]; then
		return 0
	fi

	echo "${_BOLD}${_GREEN}=> ${_NORMAL}binutils version: ${_BOLD}$BINUTILS_VERSION${_NORMAL}"
	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}downloading binutils${_NORMAL}"

	if [ ! -f "binutils-$BINUTILS_VERSION.tar.gz" ]; then
		wget -q --show-progress "http://ftpmirror.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.gz"
	fi

	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}extracting binutils${_NORMAL}"

	if [ ! -d "binutils-$BINUTILS_VERSION" ]; then
		tar xf binutils-$BINUTILS_VERSION.tar.gz $TAR_CHECKPOINT_ARG
		echo ""
		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}patching binutils${_NORMAL}"
		echo ""
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
		../binutils-$BINUTILS_VERSION/configure --target=$TARGET --prefix=$PREFIX --with-sysroot=$SYSROOT --disable-nls --disable-werror 2>&1 | pv -t -i 0.5 --name 'elapsed' > binutils-configure.log || { return 1; }

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}make${_NORMAL}"
		make -j$NUM_MAKE_JOBS all 2>&1 | pv -t -i 0.5 --name 'elapsed' > binutils-make.log || { cat binutils-make.log; return 1; }

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}install${_NORMAL}"
		make install 2>&1 | pv -t -i 0.5 --name 'elapsed' > binutils-install.log || { cat binutils-install.log; return 1; }

		echo "${_BOLD}${_GREEN}=> ${_NORMAL}${_BOLD}done!${_NORMAL}"
	popd > /dev/null
}


function build_gcc() {

	if [ $SKIP_SETUP_TOOLCHAIN_GCC == true ]; then
		return 0
	fi

	echo "${_BOLD}${_GREEN}=> ${_NORMAL}gcc version: ${_BOLD}$GCC_VERSION${_NORMAL}"
	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}downloading gcc${_NORMAL}"

	if [ ! -f "gcc-$GCC_VERSION.tar.gz" ]; then
		wget -q --show-progress "http://ftpmirror.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.gz"
	fi

	echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}extracting gcc${_NORMAL}"

	if [ ! -d "gcc-$GCC_VERSION" ]; then
		tar xf gcc-$GCC_VERSION.tar.gz $TAR_CHECKPOINT_ARG
		echo ""
		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}patching gcc${_NORMAL}"
		echo ""
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
		../gcc-$GCC_VERSION/configure --target=$TARGET --prefix=$PREFIX --with-sysroot=$SYSROOT --disable-nls --disable-werror --disable-libssp --enable-languages=c,c++ 2>&1 | pv -t -i 0.5 --name "elapsed" > gcc-configure.log || { cat gcc-configure.log; return 1; }

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}make (gcc)${_NORMAL}"
		make -j$NUM_MAKE_JOBS all-gcc 2>&1 | pv -t -i 0.5 --name "elapsed" > gcc-make.log || { cat gcc-make.log; return 1; }

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}make (libgcc)${_NORMAL}"
		make -j2 all-target-libgcc 2>&1 | pv -t -i 0.5 --name "elapsed" > libgcc-make.log || { cat libgcc-make.log; return 1; }

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}install${_NORMAL}"
		make install-gcc install-target-libgcc 2>&1 | pv -t -i 0.5 --name "elapsed" > gcc-install.log || { cat gcc-install.log; return 1; }

		echo "${_BOLD}${_GREEN}=> ${_NORMAL}${_BOLD}done!${_NORMAL}"
	popd > /dev/null
}



function build_libstdcpp() {

	pushd build-gcc > /dev/null

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}make (libstdc++)${_NORMAL}"
		make -j4 all-target-libstdc++-v3 2>&1 | pv -t -i 0.5 --name "elapsed" > libstdcpp-make.log || { cat libstdcpp-make.log; return 1; }

		echo "${_BOLD}${_BLUE}=> ${_NORMAL}${_BOLD}install${_NORMAL}"
		make install-target-libstdc++-v3 2>&1 | pv -t -i 0.5 --name "elapsed" > libstdcpp-install.log || { cat libstdcpp-install.log; return 1; }

		echo "${_BOLD}${_GREEN}=> ${_NORMAL}${_BOLD}done!${_NORMAL}"

	popd > /dev/null
}


for i in "$@"; do
	if [ $i == "--skip-libc" ]; then SKIP_SETUP_LIBC=true; fi
	if [ $i == "--skip-sysroot" ]; then SKIP_SETUP_SYSROOT=true; fi
	if [ $i == "--skip-toolchain" ]; then SKIP_SETUP_TOOLCHAIN=true; fi
	if [ $i == "--skip-diskimage" ]; then SKIP_SETUP_DISKIMAGE=true; fi
	if [ $i == "--skip-efi-toolchain" ]; then SKIP_SETUP_EFI_TOOLCHAIN=true; fi
	if [ $i == "--skip-toolchain-binutils" ]; then SKIP_SETUP_TOOLCHAIN_BINUTILS=true; fi
	if [ $i == "--skip-toolchain-gcc" ]; then SKIP_SETUP_TOOLCHAIN_GCC=true; fi
done

set -o pipefail
main "$@"












