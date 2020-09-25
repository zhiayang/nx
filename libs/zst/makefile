# Makefile

.PHONY: all install-headers

install-headers:
	@mkdir -p $(SYSROOT)/usr/include/zst/
	@cp -R $(shell find include -name "*.h") $(SYSROOT)/usr/include/zst/

all: install-headers
