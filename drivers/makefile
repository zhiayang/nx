# Makefile for Orion-X4 userspace programs
# Written in 2013

all:
	@find . -mindepth 1 -maxdepth 1 -type d -exec $(MAKE) -C {} all \;

install-headers:
	@find . -mindepth 1 -maxdepth 1 -type d -exec $(MAKE) -C {} install-headers \;
