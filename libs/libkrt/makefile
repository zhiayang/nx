# Makefile for UnixCompatLibC.
# Written in 2014


CFLAGS		= -ffreestanding -Wno-pragmas -O2 -fPIC -std=gnu11 -mno-red-zone -fno-exceptions -c
CXXFLAGS	= -ffreestanding -Wno-pragmas -O2 -fPIC -std=gnu++11 -mno-red-zone -fno-exceptions -fno-rtti -c

CSRC		= $(shell find source -name "*.c")
CXXSRC		= $(shell find source -name "*.cpp")
SSRC		= $(shell find source -name "*.s")

COBJ		= $(CSRC:.c=.c.o)
CXXOBJ		= $(CXXSRC:.cpp=.cpp.o)
SOBJ		= $(SSRC:.s=.s.o)


.PHONY: all install

headers:

install: all

all: libkrt.a

libkrt.a: $(COBJ) $(CXXOBJ) $(SOBJ)
	@echo "# $(notdir $@)"
	@$(AR) rcs libkrt.a $(COBJ) $(CXXOBJ)

%.c.o: %.c
	@$(CC) $(CFLAGS) -Iinclude -o $@ $<

%.cpp.o: %.cpp
	@$(CXX) $(CXXFLAGS) -Iinclude -o $@ $<

%.s.o: %.s
	@$(AS) $< -o $@







