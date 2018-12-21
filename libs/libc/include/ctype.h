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

#ifndef _LIBC_CTYPE_H_
#define _LIBC_CTYPE_H_

#ifdef __cplusplus
extern "C" {
#endif


inline int isblank(int c)		{ return ((c) == ' ' || (c) == '\t'); }
inline int iscntrl(int c)		{ return ((c) >= 0x0 && (c) <= 0x8); }
inline int isdigit(int c)		{ return ((c) >= '0' && (c) <= '9'); }
inline int islower(int c)		{ return ((c) >= 'a' && (c) <= 'z'); }
inline int ispunct(int c)	{ return (((c) >= 0x21 && (c) <= 0x2F) || ((c) >= 0x3A && (c) <= 0x40) || ((c) >= 0x5B && (c) <= 0x60) || ((c) >= 0x7B && (c) <= 0x7E)); }
inline int isspace(int c)	{ return ((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n' || (c) == '\f' || (c) == '\v'); }
inline int isupper(int c)	{ return ((c) >= 'A' && (c) <= 'Z'); }
inline int isalpha(int c)		{ return (islower(c) || isupper(c)); }
inline int isalnum(int c)	{ return (isalpha(c) || isdigit(c)); }
inline int isgraph(int c)	{ return (ispunct(c) || isalnum(c)); }
inline int isprint(int c)		{ return (isgraph(c) || isspace(c)); }
inline int isxdigit(int c)	{ return (isdigit(c) || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F')); }
inline int tolower(int c)	{ return (isupper(c) ? ((c) + 'a' - 'A') : (c)); }
inline int toupper(int c)	{ return (islower(c) ? ((c) + 'A' - 'a') : (c)); }

#ifdef __cplusplus
}
#endif

#endif
