# Makefile for UnixCompatLibC.
# Written in 2014

CXXFLAGS	= -Wno-pragmas -O2 -std=c++17 -fno-strict-aliasing -Iinclude -fno-rtti -Wall -Werror=return-type

CXXSRC		= $(shell find source -name "*.cpp")
CXXOBJ		= $(CXXSRC:.cpp=.cpp.o)
CXXDEPS     = $(CXXOBJ:.o=.d)

.PHONY: all

all: $(INITRD_DIR)/services/vfs-svr

$(INITRD_DIR)/services/vfs-svr: $(CXXOBJ) $(SYSROOT)/usr/lib/libc.a $(SYSROOT)/usr/lib/libnxsc.a
	@echo "# vfs-svr"
	@$(CXX) $(CXXFLAGS) -o $@ $(CXXOBJ)

%.cpp.o: %.cpp makefile
	@$(CXX) $(CXXFLAGS) -c -MMD -MP -Iinclude -I$(PROJECT_DIR)/libs/libkrt/include -o $@ $<



-include $(CXXDEPS)
