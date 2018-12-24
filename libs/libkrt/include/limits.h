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

#ifndef _STDC_LIMITS_H_
#define _STDC_LIMITS_H_

#include "machine/_sizes.h"
#include "defs/_limits.h"

#define PATH_MAX		65536

#define CHAR_BIT		__CHAR_BIT

#define SCHAR_MIN		__SCHAR_MIN
#define SCHAR_MAX		__SCHAR_MAX
#define UCHAR_MAX		__UCHAR_MAX

#define SHRT_MIN		__SHRT_MIN
#define SHRT_MAX		__SHRT_MAX
#define USHRT_MAX		__USHRT_MAX

#define INT_MIN			__INT_MIN
#define INT_MAX			__INT_MAX
#define UINT_MAX		__UINT_MAX

#define LONG_MIN		__LONG_MIN
#define LONG_MAX		__LONG_MAX
#define ULONG_MAX		__ULONG_MAX

#define LLONG_MIN		__LLONG_MIN
#define LLONG_MAX		__LLONG_MAX
#define ULLONG_MAX		__ULLONG_MAX


#ifndef WCHAR_MIN
#define WCHAR_MIN __INT_MIN
#endif

#ifndef WCHAR_MAX
#define WCHAR_MAX __INT_MAX
#endif


#endif

