# Makefile for UnixCompatLibC.
# Written in 2014

CFLAGS		= -Wno-pragmas -O2 -std=c11 -fno-strict-aliasing -c
CXXFLAGS	= -Wno-pragmas -O2 -std=c++17 -fno-strict-aliasing -fno-exceptions -fno-rtti -c

SSRC		= $(shell find source -name "*.s")
CSRC		= $(shell find source -name "*.c")
CXXSRC		= $(shell find source -name "*.cpp")

SOBJ		= $(SSRC:.s=.s.o)
COBJ		= $(CSRC:.c=.c.o)
CXXOBJ		= $(CXXSRC:.cpp=.cpp.o)

CXXDEPS     = $(CXXOBJ:.o=.d)
CDEPS       = $(COBJ:.o=.d)

_UNUSED     := $(shell cp -r include/* $(SYSROOT)/usr/include/)


.PHONY: all

all: $(SYSROOT)/usr/lib/libnxsc.a

$(SYSROOT)/usr/lib/libnxsc.a: $(COBJ) $(CXXOBJ) $(SOBJ)
	@echo "# libnxsc"
	@$(AR) rcs $(SYSROOT)/usr/lib/libnxsc.a $(SOBJ) $(COBJ) $(CXXOBJ)

%.c.o: %.c
	@echo "# $(notdir $<)"
	@$(CC) $(CFLAGS) -MMD -MP -Iinclude -o $@ $<

%.cpp.o: %.cpp
	@echo "# $(notdir $<)"
	@$(CXX) $(CXXFLAGS) -MMD -MP -Iinclude -o $@ $<

%.s.o: %.s
	@$(AS) $< -o $@


-include $(CXXDEPS)
-include $(CDEPS)





