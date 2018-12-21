/*

   Copyright 2009 Pierre KRIEGER

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
   
*/

/*
This file is here to detect the target platform
  ie. to unify pre-defined compiler-specific macros
*/

#ifndef _TARGET_H_
#define _TARGET_H_


// this is the list of macros that can be defined here
#undef _TARGET_ARM_
#undef _TARGET_IA64_
#undef _TARGET_MIPS_
#undef _TARGET_POWERPC_
#undef _TARGET_X86_


// arm
#if defined(__arm__) || defined(__TARGET_ARCH_ARM) || \
	defined(_ARM_)
#define _TARGET_ARM_
#endif


// ia-64
#if defined(__ia64__) || defined(__ia64) || \
	defined(_M_IA64)
#define _TARGET_IA64_
#endif


// mips
#if defined(__mips__) || defined(__mips) || \
	defined(__MIPS__)
#define _TARGET_MIPS_
#endif


// powerpc
#if defined(__powerpc) || defined(__powerpc__) || \
	defined(__ppc__) || defined(_M_PPC) || \
	defined(_ARCH_PPC)
#define _TARGET_POWERPC_
#endif


// x86
#if defined(i386) || defined(__i386__) ||	\
	defined(__i386) || defined(_M_IX86) || \
	defined(__X86__) || defined(_X86_) || \
	defined(__THW_INTEL__) || defined(__I86__) || \
	defined(__INTEL__)
#define _TARGET_X86_
#endif



#endif

