# Makefile for miniz
# Written in 2019

CFLAGS    = -Wno-pragmas $(KERNEL_SSE_DISABLERS) -O2 -fPIC -std=c11 -mno-red-zone -fno-exceptions -c

.PHONY: all

CSRC		= miniz.c
COBJ		= $(CSRC:.c=.c.o)

all: $(SYSROOT)/usr/lib/libminiz.a

print-miniz:
	@echo "# miniz"

$(SYSROOT)/usr/lib/libminiz.a: $(COBJ)
	@$(AR) rcs $(SYSROOT)/usr/lib/libminiz.a $(COBJ)

%.c.o: %.c makefile miniz.h | print-miniz
	@echo "  $(notdir $<)"
	@$(CC) $(CFLAGS) -o $@ $<





