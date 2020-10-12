# Makefile for Orion-X3/Orion-X4/mx and derivatives
# Written in 2011
# This makefile is licensed under the WTFPL


WARNINGS        = -Wno-padded -Wno-cast-align -Wno-unreachable-code -Wno-switch-enum -Wno-packed -Wno-missing-noreturn -Wno-float-equal -Wno-unused-macros -Werror=return-type -Wno-unused-parameter -Wno-unused-but-set-parameter -Wextra

COMMON_CFLAGS   = -m64 -Wall -fno-omit-frame-pointer -fno-strict-aliasing -ffreestanding -mno-red-zone -mcmodel=kernel -c

CXXFLAGS        = $(COMMON_CFLAGS) -Wno-old-style-cast -std=c++17 -fno-exceptions -fno-rtti
CFLAGS          = $(COMMON_CFLAGS) -std=c11


SSRC            =  $(shell find source -name "arch" -prune -o -name "*.s" -print)
SSRC            += $(shell find source/arch/$(ARCH) -iname "*.s")

CSRC            =  $(shell find source -name "arch" -prune -o -name "*.c" -print)
CSRC            += $(shell find source/arch/$(ARCH) -iname "*.c")

CXXSRC          =  $(shell find source -name "arch" -prune -o -name "*.cpp" -print)
CXXSRC          += $(shell find source/arch/$(ARCH) -iname "*.cpp")


SOBJ            = $(SSRC:.s=.s.o)
COBJ            = $(CSRC:.c=.c.o)
CXXOBJ          = $(CXXSRC:.cpp=.cpp.o)

CXXDEPS         = $(CXXOBJ:.o=.d)
CDEPS           = $(COBJ:.o=.d)

# https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html
OPTIMISATIONS   := $(KERNEL_SSE_DISABLERS) -Og -g
LDFLAGS         := -z max-page-size=0x1000 -L$(SYSROOT)/usr/lib -nostdlib

PRECOMP_HDR     := include/precompile.h
PRECOMP_GCH     := $(PRECOMP_HDR:.h=.h.gch)

DEFINES         = -DORION_KERNEL=1 -D__ARCH_x64__=1
INCLUDES        = -Iinclude -I$(PROJECT_DIR)/libs -I$(PROJECT_DIR)/libs/libkrt/include -I$(PROJECT_DIR)/libs/miniz -I$(PROJECT_DIR)/efx/zircon-efi


KERNEL          = nxkernel64

LIBKRT          = $(SYSROOT)/usr/lib/libkrt.a
LIBMINIZ        = $(SYSROOT)/usr/lib/libminiz.a

UBSAN_FLAG      = -fsanitize=undefined

.DEFAULT_GOAL = all


.PHONY: all
.PRECIOUS: $(PRECOMP_GCH)




all: $(SYSROOT)/boot/$(KERNEL)
	@mkdir -p $(SYSROOT)/usr/include/nx

print-kernel:
	@echo "# nxkernel"

$(SYSROOT)/boot/$(KERNEL): $(SOBJ) $(COBJ) $(CXXOBJ) $(LIBKRT) $(LIBMINIZ)
	@$(LD) $(LDFLAGS) -T link.ld -o $@ $(SOBJ) $(COBJ) $(CXXOBJ) -lkrt -lminiz

%.s.o: %.s makefile | print-kernel
	@echo "  $(notdir $<)"
	@$(AS) -Isource/arch/$(ARCH)/include $< -o $@

%.c.o: %.c makefile | print-kernel
	@echo "  $(notdir $<)"
	@$(CC) $(CFLAGS) $(UBSAN_FLAG) $(WARNINGS) $(DEFINES) $(INCLUDES) $(OPTIMISATIONS) -MMD -MP -o $@ $<

%.cpp.o: %.cpp makefile $(PRECOMP_GCH) | print-kernel
	@echo "  $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(UBSAN_FLAG) $(WARNINGS) $(DEFINES) $(INCLUDES) $(OPTIMISATIONS) -include $(PRECOMP_HDR) -MMD -MP -o $@ $<

%.h.gch: %.h makefile | print-kernel
	@echo "  $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(UBSAN_FLAG) $(WARNINGS) $(DEFINES) $(INCLUDES) $(OPTIMISATIONS) -MMD -MP -x c++-header -o $@ $<


-include $(PRECOMP_HDR:.h=.h.d)
-include $(CDEPS)
-include $(CXXDEPS)

clean:
	@find . -name "*.o" -delete
	@find . -name "*.h.d" -delete
	@find . -name "*.cpp.d" -delete
	@find . -name "*.h.gch" -delete
	@rm -f $(SYSROOT)/boot/$(KERNEL)











