#!/bin/sh

export QEMU="/mnt/d/programs/qemu/qemu-system-x86_64.exe"
export BOCHS="/mnt/d/programs/bochs-2.6.9-win64/bochsdbg-win64.exe"
export VIRTUALBOX="/mnt/c/Program\ Files/Oracle/VirtualBox/VirtualBoxVM.exe"

make -j12 $1
