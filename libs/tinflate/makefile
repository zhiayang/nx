# Makefile for miniz
# Written in 2019

CFLAGS    = -Wno-pragmas -O2 -fPIC -std=c11 -mno-red-zone -fno-exceptions -c

.PHONY: all

CSRC		= $(shell find . -name "*.c")
COBJ		= $(CSRC:.c=.c.o)

all: $(SYSROOT)/usr/lib/libtinflate.a

$(SYSROOT)/usr/lib/libtinflate.a: $(COBJ)
	@echo "# tinflate"
	@$(AR) rcs $(SYSROOT)/usr/lib/libtinflate.a $(COBJ)

%.c.o: %.c
	@echo "# $(notdir $<)"
	@$(CC) $(CFLAGS) -o $@ $<





