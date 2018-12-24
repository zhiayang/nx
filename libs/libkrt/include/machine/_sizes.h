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
After inclusion of this file, the following macros have to be defined:
 __CHAR_BIT__
 note: sizeof(char) is 1 by definition
	thus to have to number of bits in a short for example,
	this is calculated by __CHAR_BIT__ * __SIZEOF_SHORT__
 __SIZEOF_SHORT__
 __SIZEOF_INT__
 __SIZEOF_LONG__
 __SIZEOF_LONG_LONG__
 __SIZEOF_POINTER__
*/

#ifndef _STDC_MACHINE__TYPES_H_
#define _STDC_MACHINE__TYPES_H_

#ifndef __CHAR_BIT__
# define __CHAR_BIT__			8
#endif

#ifndef __SIZEOF_SHORT__
# define __SIZEOF_SHORT__		2
#endif

#endif
