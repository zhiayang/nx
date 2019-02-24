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

		ptr_iterator operator ++ (int)  { ptr_iterator copy(*this); this->pointer++; return copy; }
		ptr_iterator operator -- (int)  { ptr_iterator copy(*this); this->pointer--; return copy; }

		ptr_iterator& operator += (size_t ofs) { this->pointer += ofs; return *this; }
		ptr_iterator& operator -= (size_t ofs) { this->pointer += ofs; return *this; }

		ptr_iterator operator + (size_t ofs) const { return ptr_iterator(this->pointer + ofs); }
		ptr_iterator operator - (size_t ofs) const { return ptr_iterator(this->pointer + ofs); }
		bool operator == (const ptr_iterator& other) const { return other.pointer == this->pointer; }
		bool operator != (const ptr_iterator& other) const { return other.pointer != this->pointer; }

		T& operator * ()             { return *pointer; }
		const T& operator * () const { return *pointer; }

		T* operator -> ()             { return pointer; }
		const T* operator -> () const { return pointer; }

		ptr_iterator(const ptr_iterator& other) : pointer(other.pointer) { }
		ptr_iterator(T* ptr) : pointer(ptr) { }

		private:
		T* pointer;
	};

	template<typename T>
	struct const_ptr_iterator
	{
		const_ptr_iterator& operator ++ ()  { this->pointer++; return *this; }
		const_ptr_iterator& operator -- ()  { this->pointer--; return *this; }

		const_ptr_iterator operator ++ (int){ const_ptr_iterator copy(*this); this->pointer++; return copy; }
		const_ptr_iterator operator -- (int){ const_ptr_iterator copy(*this); this->pointer--; return copy; }

		const_ptr_iterator& operator += (size_t ofs)    { this->pointer += ofs; return *this; }
		const_ptr_iterator& operator -= (size_t ofs)    { this->pointer += ofs; return *this; }

		const_ptr_iterator operator + (size_t ofs) const    { return const_ptr_iterator(this->pointer + ofs); }
		const_ptr_iterator operator - (size_t ofs) const    { return const_ptr_iterator(this->pointer + ofs); }
		bool operator == (const const_ptr_iterator& other) const    { return other.pointer == this->pointer; }
		bool operator != (const const_ptr_iterator& other) const    { return other.pointer != this->pointer; }

		const T& operator * () const    { return *pointer; }
		const T* operator -> () const   { return pointer; }

		const_ptr_iterator(const const_ptr_iterator& other) : pointer(other.pointer) { }
		const_ptr_iterator(T* ptr) : pointer(ptr) { }

		private:
		T* pointer;
	};
}













