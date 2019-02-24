// circbuf.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"
#include "string.h"

#include "krt/iterators.h"

// this type kinda has strange semantics, so we just re-implement most of the things ourselves.
// there's also no real need to pull this out to a reusable implementation like we have for the
// other types, because... what other kind of circular buffers are we gonna have? any other
// abstractions should probably be built on-top of this, instead of replacing/adapting it.

namespace krt
{
	template <typename T, typename allocator, typename aborter>
	struct circularbuf
	{
		struct iterator
		{
			ptr_iterator& operator ++ ()            { this->idx = (this->idx + 1) % this->len; return *this; }
			ptr_iterator operator ++ (int)          { ptr_iterator copy(*this); ++(*this); return copy; }
			ptr_iterator& operator += (size_t ofs)  { this->idx = (this->idx + ofs) % this->len; return *this; }

			bool operator == (const ptr_iterator& other) const { return other.pointer == this->pointer && other.idx == this->idx; }
			bool operator != (const ptr_iterator& other) const { return !(*this == other); }

			T& operator * ()                { return pointer[idx]; }
			const T& operator * () const    { return pointer[idx]; }

			T* operator -> ()               { return &pointer[idx]; }
			const T* operator -> () const   { return &pointer[idx]; }

			ptr_iterator(const ptr_iterator& other) : pointer(other.pointer, other.idx, other.len) { }
			ptr_iterator(T* ptr, size_t i, size_t l) : pointer(ptr), idx(i), len(l) { }

			private:
			T* pointer;
			size_t idx;
			size_t len;
		};

		struct const_iterator
		{
			const_iterator& operator ++ ()              { this->idx = (this->idx + 1) % this->len; return *this; }
			const_iterator operator ++ (int)            { const_iterator copy(*this); ++(*this); return copy; }
			const_iterator& operator += (size_t ofs)    { this->idx = (this->idx + ofs) % this->len; return *this; }

			bool operator == (const ptr_iterator& other) const { return other.pointer == this->pointer && other.idx == this->idx; }
			bool operator != (const ptr_iterator& other) const { return !(*this == other); }

			const T& operator * () const    { return pointer[idx]; }
			const T* operator -> () const   { return &pointer[idx]; }

			const_iterator(const const_iterator& other) : pointer(other.pointer, other.idx, other.len) { }
			const_iterator(T* ptr, size_t i, size_t l) : pointer(ptr), idx(i), len(l) { }

			private:
			T* pointer;
			size_t idx;
			size_t len;
		};

		// impl details
		private:


		public:

		circularbuf(size_t max) : circularbuf(max, nullptr, 0) { }
		circularbuf(size_t max, T* p, size_t l)
		{
			this->ptr = 0;
			this->cnt = 0;

			this->cap = max;

			if(p && l)
			{
				this->cnt = l;
				this->resize(max);

				for(size_t i = 0; i < this->cnt; i++)
					this->write(p[i]);
			}
		}

		~circularbuf() { if(this->ptr) allocator::deallocate(this->ptr); }
		circularbuf(const circularbuf& other) : circularbuf(other.cap, other.ptr, other.cnt) { }

		// move
		circularbuf(circularbuf&& other)
		{
			this->cnt = other.cnt; other.cnt = 0;
			this->cap = other.cap; other.cap = 0;

			this->ptr = other.ptr; other.ptr = 0;
			this->readPtr = other.readPtr; other.readPtr = 0;
			this->writePtr = other.writePtr; other.writePtr = 0;
		}

		// copy assign
		circularbuf& operator = (const circularbuf& other)
		{
			if(this != &other)  return *this = circularbuf(other);
			else                return *this;
		}

		// move assign
		circularbuf& operator = (circularbuf&& other)
		{
			if(this != &other)
			{
				if(this->ptr) allocator::deallocate(this->ptr);

				this->cnt = other.cnt; other.cnt = 0;
				this->cap = other.cap; other.cap = 0;

				this->ptr = other.ptr; other.ptr = 0;
				this->readPtr = other.readPtr; other.readPtr = 0;
				this->writePtr = other.writePtr; other.writePtr = 0;
			}

			return *this;
		}

		void resize(size_t newmax)
		{
			auto newptr = (T*) allocator::allocate(sizeof(T) * newmax, alignof(T));

			size_t wp = 0;
			for(size_t i = 0; i < this->cnt; i++)
			{
				newptr[wp] = this->read();
				wp = (wp + 1) % newmax;
			}

			if(this->ptr) allocator::deallocate(this->ptr);

			this->ptr = newptr;
			this->cap = newmax;
			this->readIdx = 0;
			this->writeIdx = wp;
		}



		void write(const T& x)
		{
			this->writeIdx = _write(this->cap, this->ptr, this->writeIdx, x);
		}

		T read()
		{
			auto ret = this->ptr[this->readIdx];
			// this->readIdx = (hhi-s>
		}

		void write(T* xs, size_t n)
		{
			for(size_t i = 0; i < n; i++)
				this->write(xs[i]);
		}



		void clear()
		{
			this->cnt = 0;
			this->readIdx = 0;
			this->writeIdx = 0;
		}


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
		size_t readIdx = 0;
		size_t writeIdx = 0;

		size_t cnt = 0;
		size_t cap = 0;
	};
}




