# Makefile for UnixCompatLibC.
# Written in 2014


CFLAGS		= -Wno-pragmas -O2 -fPIC -std=c11 -mno-red-zone -fno-exceptions -c
CXXFLAGS	= -Wno-pragmas -O2 -fPIC -std=c++17 -mno-red-zone -fno-exceptions -fno-rtti -c

HDRS        = $(shell find . -name "*.h")

SSRC		= $(shell find . -name "*.s")
CSRC		= $(shell find . -name "*.c")
CXXSRC		= $(shell find . -name "*.cpp")

SOBJ		= $(SSRC:.s=.s.o)
COBJ		= $(CSRC:.c=.c.o)
CXXOBJ		= $(CXXSRC:.cpp=.cpp.o)

.PHONY: all

all: $(SYSROOT)/usr/lib/libc.a

print-libc:
	@echo "# libc"

$(SYSROOT)/usr/lib/libc.a: $(COBJ) $(CXXOBJ) $(SOBJ) $(HDRS)
	@$(AR) rcs $(SYSROOT)/usr/lib/libc.a $(COBJ) $(CXXOBJ) source/platform/jmp/longjmp.s.o source/platform/jmp/setjmp.s.o source/platform/support/crti.s.o source/platform/support/crtn.s.o
	@cp $(SYSROOT)/usr/lib/libc.a $(SYSROOT)/usr/lib/libg.a
	@cp source/platform/support/crt0.s.o $(SYSROOT)/usr/lib/crt0.o
	@cp source/platform/support/crti.s.o $(SYSROOT)/usr/lib/crti.o
	@cp source/platform/support/crtn.s.o $(SYSROOT)/usr/lib/crtn.o

%.c.o: %.c makefile | print-libc
	@echo "  $(notdir $<)"
	@$(CC) $(CFLAGS) -o $@ $<

%.cpp.o: %.cpp makefile | print-libc
	@echo "  $(notdir $<)"
	@$(CXX) $(CXXFLAGS) -o $@ $<

%.s.o: %.s makefile | print-libc
	@echo "  $(notdir $<)"
	@$(AS) $< -o $@







