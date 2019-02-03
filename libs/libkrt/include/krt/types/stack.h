// stack.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"
#include "string.h"

#include "krt/types/impl/arraylike.h"

namespace krt
{
	template <typename T, typename allocator, typename aborter>
	struct stack
	{
		using impl = arraylike_impl<stack, T, allocator, aborter>;
		friend impl;

		using iterator = ptr_iterator<T>;
		using const_iterator = const_ptr_iterator<T>;

		stack() : stack(nullptr, 0) { }
		stack(T* p, size_t l)
		{
			this->ptr = 0;
			this->cnt = 0;
			this->cap = 0;

			if(p && l)
			{
				this->reserve(l);
				this->cnt = l;

				for(size_t i = 0; i < this->cnt; i++)
					new (&this->ptr[i]) T(p[i]);
			}
		}

		~stack() { if(this->ptr) allocator::deallocate(this->ptr); }
		stack(const stack& other) : stack(other.ptr, other.cnt) { }

		// move
		stack(stack&& other)
		{
			this->ptr = other.ptr; other.ptr = 0;
			this->cnt = other.cnt; other.cnt = 0;
			this->cap = other.cap; other.cap = 0;
		}

		// copy assign
		stack& operator = (const stack& other)
		{
			if(this != &other)  return *this = stack(other);
			else                return *this;
		}

		// move assign
		stack& operator = (stack&& other)
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

		void reserve(size_t atleast)
		{
			auto oldptr = this->ptr;

			bool dealloc = impl::reserve(this, atleast);
			if(dealloc && oldptr)
				allocator::deallocate(oldptr);
		}

		void clear()                                        { impl::clear(this); }

		stack& push(const T& c)                             { return impl::append_element(this, c); }
		stack& push(const stack& s)                         { return impl::append(this, s); }

		T pop()
		{
			auto ret = this->top();
			this->erase_at(this->cnt - 1, 1);
			return ret;
		}

		T& top()                                            { return impl::back(this); }
		T& top() const                                      { return impl::back_const(this); }

		void erase_at(size_t idx, size_t num)               { impl::erase_at(this, idx, num); }
		bool operator == (const stack& other) const         { return impl::op_cmpeq(this, other); }
		bool operator < (const stack& other) const          { return impl::op_cmplt(this, other); }

		bool operator >  (const stack& other) const         { return impl::op_cmpgt(this, other); }
		bool operator != (const stack& other) const         { return impl::op_cmpneq(this, other); }
		bool operator >= (const stack& other) const         { return impl::op_cmpgeq(this, other); }
		bool operator <= (const stack& other) const         { return impl::op_cmpleq(this, other); }

		stack& operator += (const stack& other)             { this->append(other); return *this; }
		stack operator + (const stack& other) const         { auto copy = *this; copy.append(other); return copy; }

		iterator begin() { return iterator(this->ptr); }
		iterator end()   { return iterator(this->ptr + this->cnt); }

		const_iterator begin() const    { return const_iterator(this->ptr); }
		const_iterator end() const      { return const_iterator(this->ptr + this->cnt); }

		const_iterator cbegin() const   { return this->begin(); }
		const_iterator cend() const     { return this->end(); }



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



















