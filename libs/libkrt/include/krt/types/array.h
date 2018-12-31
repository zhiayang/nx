// array.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"
#include "string.h"

#include "krt/types/impl.h"

namespace krt
{
	template <typename T, typename allocator, typename aborter>
	struct array
	{
		using impl = arraylike_impl<array, T, allocator, aborter>;
		friend impl;

		array() : array(nullptr, 0) { }
		array(T* p, size_t l)
		{
			this->ptr = 0;
			this->cnt = 0;
			this->cap = 0;

			if(p && l)
			{
				this->reserve(l);
				this->cnt = l;

				for(size_t i = 0; i < this->cnt; i++)
					this->ptr[i] = p[i];
			}
		}

		~array() { if(this->ptr) allocator::deallocate(this->ptr); }
		array(const array& other) : array(other.ptr, other.cnt) { }

		// move
		array(array&& other)
		{
			this->ptr = other.ptr; other.ptr = 0;
			this->cnt = other.cnt; other.cnt = 0;
			this->cap = other.cap; other.cap = 0;
		}

		// copy assign
		array& operator = (const array& other)
		{
			if(this != &other)  return *this = array(other);
			else                return *this;
		}

		// move assign
		array& operator = (array&& other)
		{
			if(this != &other)
			{
				if(this->ptr) allocator::deallocate(this->ptr);

				this->ptr = other.ptr; other.ptr = 0;
				this->cnt = other.cnt; other.cnt = 0;
				this->cap = other.cap; other.cap = 0;
			}

			return *this;
		}

		size_t find(const T& c) const
		{
			for(size_t i = 0; i < this->cnt; i++)
				if(this->ptr[i] == c)
					return i;

			return -1;
		}

		// apparently we can't "using" members, so just make a new one that calls the old one.
		array subarray(size_t idx, size_t len) const        { return impl::subarray(this, idx, len); }
		void reserve(size_t atleast)                        { impl::reserve(this, atleast); }
		void clear()                                        { impl::clear(this); }

		array& append(const T& c)                           { return impl::append_element(this, c); }
		array& append(const array& s)                       { return impl::append(this, s); }

		T& front()                                          { return impl::front(this); }
		T& back()                                           { return impl::back(this); }

		const T& front() const                              { return impl::front_const(this); }
		const T& back() const                               { return impl::back_const(this); }

		void erase_at(size_t idx, size_t num)               { impl::erase_at(this, idx, num); }
		bool operator == (const array& other) const         { return impl::op_cmpeq(this, other); }
		bool operator < (const array& other) const          { return impl::op_cmplt(this, other); }

		bool operator >  (const array& other) const         { return impl::op_cmpgt(this, other); }
		bool operator != (const array& other) const         { return impl::op_cmpneq(this, other); }
		bool operator >= (const array& other) const         { return impl::op_cmpgeq(this, other); }
		bool operator <= (const array& other) const         { return impl::op_cmpleq(this, other); }


		T& operator [] (size_t idx)                         { return impl::op_subscript(this, idx); }
		const T& operator [] (size_t idx) const             { return impl::op_subscript_const(this, idx); }

		array& operator += (const array& other)             { this->append(other); return *this; }
		array operator + (const array& other) const         { auto copy = *this; copy.append(other); return copy; }

		using iterator = ptr_iterator<T>;
		iterator begin() { return iterator(this->ptr); }
		iterator end()   { return iterator(this->ptr + this->cnt); }

		T* data()                                           { return this->ptr; }
		const T* data() const                               { return (const T*) this->data; }
		bool empty() const                                  { return this->cnt == 0; }
		size_t size() const                                 { return this->cnt; }
		size_t capacity() const                             { return this->cap; }


		private:
		T* ptr = 0;
		size_t cnt = 0;
		size_t cap = 0;
	};
}


















