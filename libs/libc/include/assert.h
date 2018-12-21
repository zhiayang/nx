// assert.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#ifndef _LIBC_ASSERT_H_
#define _LIBC_ASSERT_H


/* Rid ourselves of any previous declaration of assert. */
#ifdef assert
#undef assert
#endif

/* If not debugging, we'll declare a no-operation assert macro. */
// #if defined(NDEBUG)
// #define assert(ignore) ((void) (0))
// #endif



#ifdef __cplusplus
extern "C" {
#endif

void __assert(const char* filename, unsigned long line, const char* function_name, const char* expression);

/* Otherwise, declare the normal assert macro. */
#if defined(ORION_KERNEL)

	namespace Kernel { void AssertCondition(const char*, int, const char*, const char*); }
	#define assert(invariant)	((invariant) ? (void) (0) : Kernel::AssertCondition(__FILE__, __LINE__, __PRETTY_FUNCTION__, #invariant))

#else

	#define assert(invariant)	((invariant) ? (void) (0) : __assert(__FILE__, __LINE__, __PRETTY_FUNCTION__, #invariant))

#endif


#if defined(ENABLE_PARANOID_ASSERTS)
	#define paranoid_assert(x)	assert(x)
#else
	#define paranoid_assert(x)
#endif


#endif












#ifdef __cplusplus
}
#endif
