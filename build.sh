#!/bin/sh


set -e
if grep -qEi "(Microsoft|WSL)" /proc/version &> /dev/null ; then
	make -j16 $1 QEMU="/mnt/d/programs/qemu/qemu-system-x86_64.exe" BOCHS="/mnt/d/programs/bochs-2.6.9-win64/bochsdbg-win64.exe" VIRTUALBOX="/mnt/c/Program\ Files/Oracle/VirtualBox/VirtualBoxVM.exe"
else
	make -j4 $1 QEMU="qemu-system-x86_64"
fi

