#!/bin/sh

export QEMU=qemu-system-x86_64

make -j2 $1
