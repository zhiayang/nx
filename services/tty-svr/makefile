# Makefile for UnixCompatLibC.
# Written in 2014

CXXFLAGS	= -Wno-pragmas -O2 -std=c++17 -fno-strict-aliasing -Iinclude -fno-rtti

CXXSRC		= $(shell find source -name "*.cpp")
CXXOBJ		= $(CXXSRC:.cpp=.cpp.o)
CXXDEPS     = $(CXXOBJ:.o=.d)

.PHONY: all install-headers

install-headers:
	@mkdir -p $(SYSROOT)/usr/include/svr/
	@cp -R include/export/* $(SYSROOT)/usr/include/svr/

all: $(INITRD_DIR)/services/tty-svr

$(INITRD_DIR)/services/tty-svr: $(CXXOBJ) $(SYSROOT)/usr/lib/libc.a $(SYSROOT)/usr/lib/libnxsc.a
	@echo "# tty-svr"
	@$(CXX) $(CXXFLAGS) -o $@ $(CXXOBJ)

%.cpp.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c -MMD -MP -Iinclude -o $@ $<



-include $(CXXDEPS)
