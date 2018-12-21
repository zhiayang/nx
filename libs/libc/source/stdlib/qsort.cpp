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

    stdlib/qsort.cpp
    Sort an array.

*******************************************************************************/

#include <stddef.h>

static int compare_wrapper(const void* a, const void* b, void* arg)
{
	return ((int (*)(const void*, const void*)) arg)(a, b);
}


extern "C" void qsort_r(void* base_ptr, size_t num_elements, size_t element_size, int (*compare)(const void*, const void*, void*), void* arg);

extern "C" void qsort(void* base_ptr, size_t num_elements, size_t element_size, int (*compare)(const void*, const void*))
{
	qsort_r(base_ptr, num_elements, element_size, compare_wrapper, (void*) compare);
}





