// cpu.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include "defs.h"


#ifdef __ARCH_x64__

#include "x64/cpu.h"
#include "x64/msr.h"
#include "x64/cpuid.h"

#else

#endif