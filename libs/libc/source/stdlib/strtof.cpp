// strtof.cpp
// Copyright (c) 2011 - 2014, Jonas 'Sortie' Termansen
// Licensed under the GNU LGPL.

#include "../../include/stdlib.h"



// TODO: This horribly hacky code is taken from sdlquake's common.c, which is
//       licensed under the GPL. Since most Sortix software is GPL-compatible
//       this will do for now. It's a temporary measure until I get around to
//       writing a real strtod function - most of them are true horrors.

extern "C" double strtod(const char* str, char** nptr)
{
	int sign = *str == ('-') ? (str++, -1) : 1;
	float val = 0.0;

	if(false)
	{
	out:
		if(nptr)
			*nptr = (char*) str;

		return val * sign;
	}

	if(str[0] == ('0') && (str[1] == ('x') || str[1] == ('X')))
	{
		str += 2;
		while(true)
		{
			char c = *str++;
			if(('0') <= c && c <= ('9'))
				val = val * 16 + c - ('0');

			else if(('a') <= c && c <= ('f'))
				val = val * 16 + c - ('a') + 10;

			else if(('A') <= c && c <= ('F'))
				val = val * 16 + c - ('A') + 10;

			else
				goto out;
		}
	}

	int decimal = -1;
	int total = 0;
	while(true)
	{
		char c = *str++;
		if(c == ('.'))
		{
			decimal = total;
			continue;
		}

		if(c < ('0') || c > ('9'))
			break;

		val = val * 10 + c - ('0');
		total++;
	}

	if(decimal == -1)
		goto out;

	while(decimal < total)
	{
		val /= 10;
		total--;
	}

	goto out;
}

extern "C" float strtof(const char* str, char** nptr)
{
	return (float) strtod(str, nptr);
}

