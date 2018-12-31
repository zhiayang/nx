# Makefile for UnixCompatLibC.
# Written in 2014


CFLAGS		= -ffreestanding -Wno-pragmas -O2 -fPIC -std=c11 -mno-red-zone -fno-strict-aliasing -fno-exceptions -c
CXXFLAGS	= -ffreestanding -Wno-pragmas -O2 -fPIC -std=c++17 -mno-red-zone -fno-strict-aliasing -fno-exceptions -fno-rtti -c

SSRC		= $(shell find source -name "*.s")
CSRC		= $(shell find source -name "*.c")
CXXSRC		= $(shell find source -name "*.cpp")

SOBJ		= $(SSRC:.s=.s.o)
COBJ		= $(CSRC:.c=.c.o)
CXXOBJ		= $(CXXSRC:.cpp=.cpp.o)

CXXDEPS     = $(CXXOBJ:.o=.d)
CDEPS       = $(COBJ:.o=.d)

.PHONY: all


all: $(SYSROOT)/usr/lib/libkrt.a

$(SYSROOT)/usr/lib/libkrt.a: $(COBJ) $(CXXOBJ) $(SOBJ)
	@echo "# libkrt"
	@$(AR) rcs $(SYSROOT)/usr/lib/libkrt.a $(COBJ) $(CXXOBJ)

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





