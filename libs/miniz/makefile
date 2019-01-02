# Makefile for miniz
# Written in 2019

CFLAGS		= -Wno-pragmas -O2 -fPIC -std=c11 -mno-red-zone -fno-exceptions -c -DMINIZ_NO_TIME -DMINIZ_NO_STDIO -DMINIZ_NO_ARCHIVE_APIS -DMINIZ_NO_ARCHIVE_WRITING_APIS -DMINIZ_NO_ZLIB_COMPATIBLE_NAMES -DMINIZ_NO_MALLOC

.PHONY: all

all: $(SYSROOT)/usr/lib/libminiz.a

$(SYSROOT)/usr/lib/libminiz.a: miniz.c.o
	@echo "# libc"
	@$(AR) rcs $(SYSROOT)/usr/lib/libminiz.a $(COBJ)

miniz.c.o: miniz.c
	@echo "# $(notdir $<)"
	@$(CC) $(CFLAGS) -o $@ $<







