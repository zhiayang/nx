# Makefile for UnixCompatLibC.
# Written in 2014

CXXFLAGS	= -Wno-pragmas -O2 -std=c++17 -fno-strict-aliasing -g -fno-omit-frame-pointer -Wall

.PHONY: all install-headers

install-headers:

all: $(INITRD_DIR)/drivers/placebo

$(INITRD_DIR)/drivers/placebo: source/placebo.cpp $(SYSROOT)/usr/lib/libc.a $(SYSROOT)/usr/lib/libnxsc.a
	@echo "# placebo"
	@$(CXX) $(CXXFLAGS) -o $@ source/placebo.cpp


