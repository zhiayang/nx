# [nx] #

The sixth rewrite of what never was, OrionOS. x86_64 kernel.


## Current Features ##
- EFI loader (efxloader) and BIOS loader (bfxloader, running on top of [BOOTBOOT](https://gitlab.com/bztsrc/bootboot))
- Higher-half 64-bit kernel
- tarfs initrd, basic VFS
- Some level of userspace, with rudimentary (non-signalling) IPC


## Trying out [nx] ##
#### Required Software ####
- C++ compiler (that targets your own platform)
- `pv`, `mtools`, `parted`, `fdisk`, `mkfs.vfat` (typically packaged as `dosfstools`), `unzip`
- On OS X, use [Homebrew](https://brew.sh) to install `gptfdisk` (the included `fdisk` only works for MBR disks, which is so 1990s)
- A Unix system -- OS X and Linux will work, Windows Subsystem for Linux will work as well. Unlikely to work on things like mingw/cygwin.
- About 150 MB of free space.


#### Building the toolchain ####
- Run the `./bootstrap.sh` script
- Everything will be done automatically


#### Building the kernel ####
- Once the bootstrap script is done, you should be able to run 'make' in the root directory and have it run, using QEMU.
- It currently does nothing spectacular. Possible epilepsy warning.



-----------------------

## License ##

The licenses for the various components of \[nx\] can be found in `LICENSE.md`, including those of libraries and external dependencies.





