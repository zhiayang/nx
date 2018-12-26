# [nx] #

The sixth rewrite of what never was, OrionOS.


## Current Features ##
- nothing
- half-finished EFI loader


## Trying out [nx] ##
#### Required Software ####
- C++ compiler (that targets your own platform)
- `pv`, `mtools`, `parted`, `fdisk`, `mkfs.vfat` (typically packaged as `dosfstools`), `unzip`
- A Unix system -- OSX and Linux will work, Windows Subsystem for Linux will work as well. Unlikely to work on things like mingw/cygwin.


#### Building [mx]'s toolchain ####
- Run the ./bootstrap.sh script
- Everything will be done automatically


#### Building the kernel ####
- Once the bootstrap script is done, you should be able to run 'make' in the root directory and have it run.
- It currently does nothing.



-----------------------

## License ##

The licenses for the various components of \[nx\] can be found in `LICENSE.md`, including those of our various libraries and dependencies.





