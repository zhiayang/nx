# [nx] #

The sixth rewrite of what never was, OrionOS.


## Current Features ##
- EFI loader (efxloader)
- Higher-half 64-bit kernel
- PMM, VMM, heap allocator
- tarfs initrd, basic VFS


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
- It currently does nothing spectacular.



-----------------------

## License ##

The licenses for the various components of \[nx\] can be found in `LICENSE.md`, including those of our various libraries and dependencies.





