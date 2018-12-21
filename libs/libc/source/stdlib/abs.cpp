// abs.cpp
// Copyright (c) 2011 - 2014, Jonas 'Sortie' Termansen
// Licensed under the GNU LGPL.

extern "C" int abs(int j)
{
	return (j < 0) ? (-j) : j;
}

extern "C" long int labs(long int j)
{
	return (j < 0) ? (-j) : j;
}

extern "C" long long int llabs(long long int j)
{
	return (j < 0) ? (-j) : j;
}
