# Makefile for UnixCompatLibC.
# Written in 2014


COMMON_CLFAGS   = -ffreestanding -Wno-pragmas -O2 $(KERNEL_SSE_DISABLERS) -fPIC -mno-red-zone -fno-strict-aliasing -c

CFLAGS		    = $(COMMON_CLFAGS) -std=c11
CXXFLAGS	    = $(COMMON_CLFAGS) -std=c++17 -fno-exceptions -fno-rtti

SSRC		    = $(shell find source -name "*.s")
CSRC		    = $(shell find source -name "*.c")
CXXSRC		    = $(shell find source -name "*.cpp")

SOBJ		    = $(SSRC:.s=.s.o)
COBJ		    = $(CSRC:.c=.c.o)
CXXOBJ		    = $(CXXSRC:.cpp=.cpp.o)

CXXDEPS         = $(CXXOBJ:.o=.d)
CDEPS           = $(COBJ:.o=.d)

.PHONY: all


all: $(SYSROOT)/usr/lib/libkrt.a

print-libkrt:
	@echo "# libkrt"

$(SYSROOT)/usr/lib/libkrt.a: $(COBJ) $(CXXOBJ) $(SOBJ)
	@$(AR) rcs $(SYSROOT)/usr/lib/libkrt.a $(SOBJ) $(COBJ) $(CXXOBJ)

%.c.o: %.c makefile | print-libkrt
	@echo "  $(notdir $<)"
	@$(CC) $(CFLAGS) -MMD -MP -Iinclude -o $@ $<

%.cpp.o: %.cpp makefile | print-libkrt
	@echo "  $(notdir $<)"
	@$(CXX) $(CXXFLAGS) -MMD -MP -Iinclude -o $@ $<

%.s.o: %.s makefile | print-libkrt
	@echo "  $(notdir $<)"
	@$(AS) $< -o $@


-include $(CXXDEPS)
-include $(CDEPS)





