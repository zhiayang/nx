// array.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"
#include "string.h"

#include "krt/macros.h"

#include "krt/meta.h"
#include "krt/iterators.h"


namespace krt
{
	struct kernel_aborter;
	struct kernel_allocator;

	template <typename T, typename allocator = kernel_allocator, typename aborter = kernel_aborter>
	struct array
	{
		array() : ptr(0), cnt(0), cap(0) { }

		array(const T* p, size_t l)
		{
			this->ptr = 0;
			this->cnt = 0;
			this->cap = 0;

			if(p && l)
			{
				this->cnt = 0;
				this->cap = 0;

				this->reserve(l + 1);
				this->cnt = l;

				for(size_t i = 0; i < this->cnt; i++)
					this->ptr[i] = p[i];

				this->set_last_if_char();
			}
		}

		~array()
		{
			if(this->ptr)
				allocator::deallocate(this->ptr);
		}

		// copy
		array(const array& other) : array(other.ptr, other.cnt)
		{
		}

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






		void reserve(size_t atleast)
		{
			if(this->cap >= atleast) return;
			size_t newsz = __max(16, __max(atleast, (this->cap * 3) / 2));

			// if T is like char16_t or some shit, we need to account for it
			auto newptr = (T*) allocator::allocate(newsz * sizeof(T));
			if(!newptr) aborter::abort("alloc() returned null!");

			// ugh.
			for(size_t i = 0; i < this->cnt; i++)
				newptr[i] = this->ptr[i];

			allocator::deallocate(this->ptr);

			this->ptr = newptr;
			this->cap = newsz;
		}

		size_t capacity() const { return this->cap; }
		size_t count() const    { return this->cnt; }
		bool empty() const      { return this->cnt == 0; }
		void clear()            { this->cnt = 0; }

		T* data()               { return this->ptr; }
		const T* data() const   { return (const T*) this->ptr; }


		array subarray(size_t idx, size_t len) const
		{
			if(idx >= this->cnt || idx + len > this->cnt)
				aborter::abort("parameters to array::subarray out of range! wanted (%zu, %zu), length is only %d", idx, len, this->cnt);

			// ok, we should be good.
			return array(this->ptr + idx, len);
		}

		array& append(const T& c)
		{
			this->reserve(this->cnt + 1 + 1);

			this->ptr[this->cnt] = c;
			this->cnt++;

			this->set_last_if_char();
			return *this;
		}

		array& append(const array& s)
		{
			this->reserve(this->cnt + s.cnt + 1);

			for(size_t i = this->cnt; i < this->cnt + s.cnt; i++)
				this->ptr[i] = s.ptr[i - this->cnt];

			this->cnt += s.cnt;

			this->set_last_if_char();
			return *this;
		}











		bool operator == (const array& other) const
		{
			if(this->cnt != other.cnt) return false;
			for(size_t i = 0; i < this->cnt; i++)
			{
				if(this->ptr[i] != other->ptr[i])
					return false;
			}

			return true;
		}

		bool operator < (const array& other) const
		{
			if(this->cnt != other.cnt) return false;

			// sadly we do not have the min macro.
			for(size_t i = 0; i < (other.cnt < this->cnt ? other.cnt : this->cnt); i++)
			{
				if(this->ptr[i] < other.ptr[i])
					return true;

				else if(this->ptr[i] > other.ptr[i])
					return false;
			}

			// well they were equal -- we return the shorter array.
			return this->cnt < other.cnt;
		}

		bool operator != (const array& other) const { return !(*this == other); }
		bool operator >= (const array& other) const { return !(*this < other); }
		bool operator <= (const array& other) const { return !(other < *this); }
		bool operator >  (const array& other) const { return (other < *this); }


		T& operator [] (size_t idx)
		{
			if(idx >= this->cnt) aborter::abort("index out of range! wanted %zu, only have %zu", idx, this->cnt);
			return this->ptr[idx];
		}

		const T& operator [] (size_t idx) const
		{
			if(idx >= this->cnt) aborter::abort("index out of range! wanted %zu, only have %zu", idx, this->cnt);
			return this->ptr[idx];
		}

		array operator + (const array& other) const
		{
			auto copy = *this;
			copy.append(other);

			return copy;
		}

		array& operator += (const array& other)
		{
			this->append(other);
			return *this;
		}


		using iterator = ptr_iterator<T>;
		iterator begin() { return iterator(this->ptr); }
		iterator end()   { return iterator(this->ptr + this->cnt); }

		private:
		size_t cnt = 0;
		size_t cap = 0;
		T* ptr = nullptr;

		void set_last_if_char()
		{
			if constexpr (krt::is_same<T, char>())
				memset(this->ptr + this->cnt, 0, sizeof(T));
		}
	};
}



















