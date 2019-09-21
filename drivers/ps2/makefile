# Makefile for UnixCompatLibC.
# Written in 2014

CXXFLAGS	= -Wno-pragmas -O2 -std=c++17 -fno-strict-aliasing

.PHONY: all

all: $(INITRD_DIR)/drivers/ps2

$(INITRD_DIR)/drivers/ps2: source/ps2.cpp source/keyboard.cpp $(SYSROOT)/usr/lib/libc.a $(SYSROOT)/usr/lib/libnxsc.a
	@echo "# ps2"
	@$(CXX) $(CXXFLAGS) -o $@ -lnxsc source/ps2.cpp source/keyboard.cpp


