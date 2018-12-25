// iterators.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>

#include "stdint.h"

namespace krt
{
	template<typename T>
	struct ptr_iterator
	{
		ptr_iterator& operator ++ () { this->pointer++; return *this; }
		ptr_iterator& operator -- () { this->pointer--; return *this; }
		ptr_iterator& operator += (size_t ofs) { this->pointer += ofs; return *this; }
		ptr_iterator& operator -= (size_t ofs) { this->pointer += ofs; return *this; }

		ptr_iterator operator + (size_t ofs) const { return ptr_iterator(this->pointer + ofs); }
		ptr_iterator operator - (size_t ofs) const { return ptr_iterator(this->pointer + ofs); }
		bool operator == (const ptr_iterator& other) const { return other.pointer == this->pointer; }
		bool operator != (const ptr_iterator& other) const { return other.pointer != this->pointer; }

		T& operator * ()             { return *pointer; }
		const T& operator * () const { return *pointer; }


		ptr_iterator(const ptr_iterator& other) : pointer(other.pointer) { }
		ptr_iterator(T* ptr) : pointer(ptr) { }

		private:
		T* pointer;
	};
}