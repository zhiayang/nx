// cdefs.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


/* Preprocessor trick to turn anything into a string. */
#define __STRINGIFY(x) #x

/* Issue warning when this is used, except in defines, where the warning is
   inserted whenever the macro is expanded. This can be used to deprecated
   macros - and it happens on preprocessor level - so it shouldn't change any
   semantics of any code that uses such a macro. The argument msg should be a
   string that contains the warning. */
#define __PRAGMA_WARNING(msg) _Pragma(__STRINGIFY(GCC warning msg))

/* C++ needs to know that types and declarations are C, not C++. */
#ifdef __cplusplus
	#define __BEGIN_DECLS	extern "C" {
	#define __END_DECLS		}
#else
	#define __BEGIN_DECLS
	#define __END_DECLS
#endif

#define restrict __restrict
#define __pure2 __attribute__((__const__))
