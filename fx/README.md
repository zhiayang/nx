# [fx] #

The loader for [mx].

## Current Features ##
- It's practically a stripped-down, low-tech version of [mx].
- Fun fact: code for the PMM was taken from Orion-X3, because [mx]'s PMM uses a heap
- Fun fact (2): [fx] does not have a heap memory allocator.


## Loading Process ##
- Start.s from [mx] was ripped from its home in source/Kernel and pretty much remained the same
- Please note that [mx] relies on its loader to set up at least 16MB of identity mapped memory from 0x0
- Long mode and a basic GDT are set up. Interrupts remain disabled.
- Jump to ELF entry point.


## Building [fx] ##
- You should not (and in fact cannot) build it separately from the kernel.
- It will be built automatically as part of [mx]'s build process.
