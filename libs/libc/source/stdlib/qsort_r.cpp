/*******************************************************************************

    Copyright(C) Jonas 'Sortie' Termansen 2012, 2014.

    This file is part of the Sortix C Library.

    The Sortix C Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    The Sortix C Library is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the Sortix C Library. If not, see <http://www.gnu.org/licenses/>.

    stdlib/qsort_r.cpp
    Sort an array.

*******************************************************************************/

#include "../../include/stdlib.h"
#include "../../include/string.h"

static void memswap(unsigned char* a, unsigned char* b, size_t size)
{
	unsigned char tmp;
	for(size_t i = 0; i < size; i++)
	{
		tmp = a[i];
		a[i] = b[i];
		b[i] = tmp;
	}
}

static unsigned char* array_index(unsigned char* base, size_t element_size, size_t index)
{
	return base + element_size * index;
}

static size_t partition(unsigned char* base, size_t element_size, size_t num_elements, size_t pivot_index, int (*compare)(const void*, const void*, void*), void* arg)
{
	if(pivot_index != num_elements - 1)
	{
		unsigned char* pivot = array_index(base, element_size, pivot_index);
		unsigned char* other = array_index(base, element_size, num_elements - 1);
		memswap(pivot, other, element_size);
		pivot_index = num_elements - 1;
	}

	size_t store_index = 0;
	for(size_t i = 0; i < num_elements - 1; i++)
	{
		unsigned char* pivot = array_index(base, element_size, pivot_index);
		unsigned char* value = array_index(base, element_size, i);
		if(compare(value, pivot, arg) <= 0)
		{
			unsigned char* other = array_index(base, element_size, store_index);
			if(value != other)
				memswap(value, other, element_size);
			store_index++;
		}
	}

	unsigned char* pivot = array_index(base, element_size, pivot_index);
	unsigned char* value = array_index(base, element_size, store_index);
	memswap(pivot, value, element_size);

	return store_index;
}

extern "C" void qsort_r(void* base_ptr, size_t num_elements, size_t element_size, int (*compare)(const void*, const void*, void*), void* arg)
{
	unsigned char* base = (unsigned char*) base_ptr;

	if(!element_size || num_elements < 2)
		return;

	size_t pivot_index = num_elements / 2;
	pivot_index = partition(base, element_size, num_elements, pivot_index, compare, arg);

	if(2 <= pivot_index)
		qsort_r(base, pivot_index, element_size, compare, arg);

	if(2 <= num_elements - (pivot_index + 1))
		qsort_r(array_index(base, element_size, pivot_index + 1), num_elements - (pivot_index + 1), element_size, compare, arg);
}
