# Makefile for UnixCompatLibC.
# Written in 2014

CXXFLAGS	= -Wno-pragmas -O2 -std=c++17 -fno-strict-aliasing -Wall

.PHONY: all install-headers

install-headers:

all: $(INITRD_DIR)/drivers/ps2

$(INITRD_DIR)/drivers/ps2: source/tarfs.cpp $(SYSROOT)/usr/lib/libc.a $(SYSROOT)/usr/lib/libnxsc.a
	@echo "# tarfs"
	@$(CXX) $(CXXFLAGS) -o $@ source/tarfs.cpp


