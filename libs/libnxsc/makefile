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


.PHONY: all

all: $(SYSROOT)/usr/lib/libnxsc.a

print-libnxsc:
	@echo "# libnxsc"

$(SYSROOT)/usr/lib/libnxsc.a: $(SOBJ) $(COBJ) $(CXXOBJ)
	@$(AR) rcs $(SYSROOT)/usr/lib/libnxsc.a $(SOBJ) $(COBJ) $(CXXOBJ)

%.c.o: %.c makefile | print-libnxsc
	@echo "  $(notdir $<)"
	@$(CC) $(CFLAGS) -MMD -MP -Iinclude -o $@ $<

%.cpp.o: %.cpp makefile | print-libnxsc
	@echo "  $(notdir $<)"
	@$(CXX) $(CXXFLAGS) -MMD -MP -Iinclude -o $@ $<

%.s.o: %.s makefile | print-libnxsc
	@echo "  $(notdir $<)"
	@$(AS) $< -o $@


-include $(CXXDEPS)
-include $(CDEPS)





