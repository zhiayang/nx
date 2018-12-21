# Makefile for Orion-X3/Orion-X4/mx and derivatives
# Written in 2011
# This makefile is licensed under the WTFPL

export QEMU				:= qemu-system-x86_64
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
export MOUNTPATH		:= $(shell tools/getpath.sh)
export COMPRESS_UTIL	:= $(shell pwd)/tools/heatshrink

# we use clang only for the kernel, don't pollute makefiles
CXX_				= clang++
FLXC				= $(SYSROOT)/usr/local/bin/flaxc
GCCVERSION			= 8.2.0

WARNINGS			= -Wno-padded -Wno-c++98-compat-pedantic -Wno-c++98-compat -Wno-cast-align -Wno-unreachable-code -Wno-gnu -Wno-missing-prototypes -Wno-switch-enum -Wno-packed -Wno-missing-noreturn -Wno-float-equal -Wno-old-style-cast -Wno-exit-time-destructors -Wno-unused-macros -Wno-global-constructors -Wno-documentation-unknown-command -Wno-reserved-id-macro -Wno-c99-extensions

CXXFLAGS			= -m64 -g -Weverything -msse3 -integrated-as -O2 -fno-omit-frame-pointer -fno-strict-aliasing -std=c++17 -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -I./source/Kernel/include -I./Libraries/Iris/HeaderFiles -I./Libraries/ -I$(SYSROOT)/usr/include -I$(SYSROOT)/usr/include/c++ -DORION_KERNEL=1 -target x86_64-elf -mcmodel=kernel -c

LDFLAGS				= -z max-page-size=0x1000 -L$(SYSROOT)/usr/lib

MEMORY				= 1024

SSRC				= $(shell find source -iname "*.s")
CXXSRC				= $(shell find source -iname "*.cpp")

SOBJ				= $(SSRC:.s=.s.o)
CXXOBJ				= $(CXXSRC:.cpp=.cpp.o)

CXXDEPS				= $(CXXOBJ:.o=.d)

.DEFAULT_GOAL = all
-include $(CXXDEPS)


NUMFILES			= $$(($(words $(CXXSRC)) + $(words $(SSRC))))



LIBRARIES			= -liris -lrdestl -lkstl
OUTPUT				= build/kernel64.elf



QEMU_FLAGS			= -s -vga std -no-reboot -m $(MEMORY) -rtc base=utc -net nic,model=rtl8139 -net user -net dump,file=build/netdump.wcap     -drive file=build/disk.img,format=raw

.PHONY: builduserspace buildlib mountdisk clean all cleandisk copyheader

run: build
	@$(QEMU) -serial file:"build/serialout.log" $(QEMU_FLAGS) -monitor stdio

all: $(OUTPUT)
	@# unmount??
	@tools/unmountdisk.sh

	@echo "# Starting QEMU"
	@$(QEMU) -serial stdio $(QEMU_FLAGS) | tee build/serialout.log
	-@rm -f build/.dmf

	@# mount the disk again for inspection.
	@tools/mountdisk.sh


debug: $(OUTPUT)
	@# unmount??
	@tools/unmountdisk.sh

	@echo "# Starting QEMU"
	@$(QEMU) -serial stdio -S $(QEMU_FLAGS) | tee build/serialout.log
	-@rm -f build/.dmf

	@# mount the disk again for inspection.
	@tools/mountdisk.sh



build: $(OUTPUT)
	# built



$(OUTPUT): mountdisk copyheader $(SYSROOT)/usr/lib/%.a $(SOBJ) $(CXXOBJ) $(FLAXMAINOBJ) builduserspace
	@echo "\n# Linking [mx] object files"
	@$(LD) $(LDFLAGS) -T kernel.ld -o build/kernel64.elf $(shell find source/Kernel -name "*.o") $(LIBRARIES)

	@echo "# Building [fx] loader"
	@$(LD) $(LDFLAGS) -T loader.ld -o build/fxloader64.elf source/Loader/Start.s.o $(shell find source/Loader -name "*.o" ! -name "Start.s.o")

	@echo # Performing objcopy
	@$(OBJCOPY) -g -O elf32-i386 build/fxloader64.elf build/fxloader.oxf
	@cp build/fxloader.oxf $(shell tools/getpath.sh)/boot/fxloader.oxf

	@echo # Copying any extra files
	@cp -R build/diskroot/* $(shell tools/getpath.sh)/ || :




	@# use objcopy to strip debug symbols from the final executable -- saves about 1.3mb

	@$(OBJCOPY) -g $(OUTPUT) build/kernel64.uncompressed
	@# @cp $(OUTPUT) build/kernel64.uncompressed




	@echo "# Compressing kernel"

	@$(COMPRESS_UTIL) -e -w 14 -l 6 build/kernel64.uncompressed build/kernel64
	@# @cp build/kernel64.uncompressed build/kernel64






	@printf "0: %.16x" $(shell cat build/kernel64.uncompressed | wc -c) | xxd -r -g 0 >> .size_file
	@cat build/kernel64 >> .size_file
	@mv .size_file build/kernel64

	@echo "# Compressed"

	@cp build/kernel64.uncompressed $(shell tools/getpath.sh)/System/Library/CoreServices/kernel64.uncompressed
	@cp build/kernel64 $(shell tools/getpath.sh)/System/Library/CoreServices/kernel64



%.s.o: %.s
	@if [ ! -a build.dmf ]; then tools/updatebuild.sh; fi;
	@touch build/.dmf

	@$(AS) $< -o $@

	@$(eval DONEFILES += "S")
	@printf "\r                                               \r$(words $(DONEFILES)) / $(NUMFILES) ($(notdir $<))"


%.cpp.o: %.cpp
	@if [ ! -a build.dmf ]; then tools/updatebuild.sh; fi;
	@touch build/.dmf

	@$(CXX_) $(CXXFLAGS) $(WARNINGS) -MMD -MP -o $@ $<

	@$(eval DONEFILES += "CPP")
	@printf "\r                                               \r$(words $(DONEFILES)) / $(NUMFILES) ($(notdir $<))"


builduserspace:
	@printf "\n# Building userspace applications\n"
	@$(MAKE) -C applications/

copyheader:
	@mkdir -p $(SYSROOT)/usr/lib
	@mkdir -p $(SYSROOT)/usr/include/c++
	@rsync -cmar Libraries/libc/include/* $(SYSROOT)/usr/include/
	@rsync -cmar Libraries/libm/include/* $(SYSROOT)/usr/include/
	@rsync -cmar Libraries/Iris/HeaderFiles/* $(SYSROOT)/usr/include/iris/
	@rsync -cmar Libraries/libsyscall/*.h $(SYSROOT)/usr/include/sys/
	@rsync -cmar $(TOOLCHAIN)/x86_64-orionx/include/c++/$(GCCVERSION)/* $(SYSROOT)/usr/include/c++/
	@rsync -cmar $(SYSROOT)/usr/include/c++/x86_64-orionx/bits/* $(SYSROOT)/usr/include/c++/bits/
	@cp $(TOOLCHAIN)/x86_64-orionx/lib/*.a $(SYSROOT)/usr/lib/
	@cp $(TOOLCHAIN)/lib/gcc/x86_64-orionx/$(GCCVERSION)/libgcc.a $(SYSROOT)/usr/lib/


buildlib: $(SYSROOT)/usr/lib/%.a
	@:

$(SYSROOT)/usr/lib/%.a:
	@echo "# Building Libraries"
	@$(MAKE) -C Libraries/

mountdisk:
	@tools/mountdisk.sh

cleandisk:
	@find $(MOUNTPATH) -name "*.mxa" | xargs rm -f
	@find $(MOUNTPATH) -name "*.x" | xargs rm -f

cleanlib:
	@find Libraries -name "*.a" | xargs rm -f
	@find Libraries -name "*.o" | xargs rm -f
	@find Libraries -name "*.cpp.d" | xargs rm -f

cleanall: cleandisk clean
	@echo "# Cleaning directory tree"
	@find Libraries -name "*.o" | xargs rm -f
	@find Libraries -name "*.cpp.d" | xargs rm -f
	@find Libraries -name "*.a" | xargs rm -f
	@find applications -name "*.o" | xargs rm -f

# haha
clena: clean
clean:
	@find source -name "*.o" | xargs rm -f
	@find source -name "*.cpp.d" | xargs rm -f









