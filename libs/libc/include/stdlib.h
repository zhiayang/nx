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

#ifndef _STDC_STDLIB_H_
#define _STDC_STDLIB_H_

#include <stddef.h>
#include "time.h"
#include "sys/types.h"
#include "defs/_file.h"
#include "defs/_func.h"

#include "malloc.h"

#ifdef __cplusplus
extern "C" {
#endif

extern func_void_void_t __exitfunc;
extern tm __tm;



#define	RAND_MAX		(__INT_MAX__ - 1)

typedef struct	{ int quot; int rem; }			div_t;
typedef struct	{ long quot; long rem; }		ldiv_t;
typedef struct	{ long long quot; long long rem; }	lldiv_t;






// misc
int atexit(func_void_void_t func);
void exit(int ret);


// incomplete
char* getenv(const char* env);
int system(const char* cmd);

// #ifdef wchar_t
size_t mbstowcs(wchar_t* dest, const char* src, size_t max);
int mbtowc(wchar_t* pwc, const char* pmb, size_t max);
// #endif




#define __min(a, b)		(a > b ? b : a)
#define __max(a, b)		(a > b ? a : b)
#define __abs(x)		(x < 0 ? -x : x)










// bunch of shit
double			atof	(const char* nptr);
int			atoi	(const char* nptr);
long			atol	(const char* nptr);
long long		atoll	(const char* nptr);
double			strtod	(const char* nptr, char** endptr);
float			strtof	(const char* nptr, char** endptr);
long double		strtold	(const char* nptr, char** endptr);
long			strtol	(const char* nptr, char** endptr, int base);
long long		strtoll	(const char* nptr, char** endptr, int base);
unsigned long		strtoul	(const char* nptr, char** endptr, int base);
unsigned long long	strtoull	(const char* nptr, char** endptr, int base);
int			rand	();
void			srand	(unsigned int seed);
void*			bsearch(const void* key, const void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*));
void			qsort	(void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*));
int			abs	(int j);
long int			labs	(long int j);
long long		llabs	(long long int j);
div_t			div	(int numer, int denom);
ldiv_t			ldiv	(long numer, long denom);
lldiv_t			lldiv	(long long numer, long long denom);
int			mblen	(const char* s, size_t n);
void			abort	();

char* itoa(int num, char* dest, int base);
char* ltoa(long num, char* dest, int base);
char* lltoa(long long num, char* dest, int base);



#ifdef __cplusplus
}
#endif


#endif
