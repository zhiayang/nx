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

.PHONY: all copyhdrs

all: $(SYSROOT)/usr/lib/libc.a
	@cp -r include/* $(SYSROOT)/usr/include/

$(SYSROOT)/usr/lib/libc.a: copyhdrs $(COBJ) $(CXXOBJ) $(SOBJ) $(HDRS)
	@echo "# libc"
	@$(AR) rcs $(SYSROOT)/usr/lib/libc.a $(COBJ) $(CXXOBJ) source/platform/jmp/longjmp.s.o source/platform/jmp/setjmp.s.o source/platform/support/crti.s.o source/platform/support/crtn.s.o source/syscall/helper.s.o
	@cp $(SYSROOT)/usr/lib/libc.a $(SYSROOT)/usr/lib/libg.a
	@cp source/platform/support/crt0.s.o $(SYSROOT)/usr/lib/crt0.o
	@cp source/platform/support/crti.s.o $(SYSROOT)/usr/lib/crti.o
	@cp source/platform/support/crtn.s.o $(SYSROOT)/usr/lib/crtn.o

%.c.o: %.c
	@echo "# $(notdir $<)"
	@$(CC) $(CFLAGS) -o $@ $<

%.cpp.o: %.cpp
	@echo "# $(notdir $<)"
	@$(CXX) $(CXXFLAGS) -o $@ $<

%.s.o: %.s
	@echo "# $(notdir $<)"
	@$(AS) $< -o $@







