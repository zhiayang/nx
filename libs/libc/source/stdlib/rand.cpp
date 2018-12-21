// rand.cpp
// Copyright (c) 2011 - 2014, Jonas 'Sortie' Termansen
// Licensed under the GNU LGPL.

#include "../../include/stdlib.h"

static unsigned long _rand_value = 1;

extern "C" int rand()
{
	_rand_value = _rand_value * 110351524 + 12345;
	return (int) (_rand_value % (RAND_MAX + 1));
}

extern "C" void srand(unsigned int seed)
{
	_rand_value = seed;
}

