# Makefile for miniz
# Written in 2019

CFLAGS    = -Wno-pragmas $(KERNEL_SSE_DISABLERS) -O2 -fPIC -std=c11 -mno-red-zone -fno-exceptions -c

.PHONY: all

CSRC		= $(shell find . -name "*.c")
COBJ		= $(CSRC:.c=.c.o)

all: $(SYSROOT)/usr/lib/libtinflate.a

print-tinflate:
	@echo "# tinflate"

$(SYSROOT)/usr/lib/libtinflate.a: $(COBJ)
	@$(AR) rcs $(SYSROOT)/usr/lib/libtinflate.a $(COBJ)

%.c.o: %.c makefile | print-tinflate
	@echo "  $(notdir $<)"
	@$(CC) $(CFLAGS) -o $@ $<





