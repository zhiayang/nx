#!/bin/sh

export QEMU="/mnt/d/programs/qemu/qemu-system-x86_64.exe"
export VIRTUALBOX="/mnt/c/Program\ Files/Oracle/VirtualBox/VBoxManage.exe"

make -j8 $1
