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
			iterator& operator ++ ()            { this->idx = (this->idx + 1) % this->len; return *this; }
			iterator operator ++ (int)          { iterator copy(*this); ++(*this); return copy; }
			iterator& operator += (size_t ofs)  { this->idx = (this->idx + ofs) % this->len; return *this; }

			bool operator == (const iterator& other) const { return other.pointer == this->pointer && other.idx == this->idx; }
			bool operator != (const iterator& other) const { return !(*this == other); }

			T& operator * ()                { return pointer[idx]; }
			const T& operator * () const    { return pointer[idx]; }

			T* operator -> ()               { return &pointer[idx]; }
			const T* operator -> () const   { return &pointer[idx]; }

			iterator(const iterator& other) : pointer(other.pointer, other.idx, other.len) { }
			iterator(T* ptr, size_t i, size_t l) : pointer(ptr), idx(i), len(l) { }

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

			bool operator == (const const_iterator& other) const { return other.pointer == this->pointer && other.idx == this->idx; }
			bool operator != (const const_iterator& other) const { return !(*this == other); }

			const T& operator * () const    { return pointer[idx]; }
			const T* operator -> () const   { return &pointer[idx]; }

			const_iterator(const const_iterator& other) : pointer(other.pointer, other.idx, other.len) { }
			const_iterator(T* ptr, size_t i, size_t l) : pointer(ptr), idx(i), len(l) { }

			private:
			T* pointer;
			size_t idx;
			size_t len;
		};

		using elem_type = T;
		static constexpr size_t DEFAULT_SIZE = 32;

		circularbuf() : circularbuf(DEFAULT_SIZE, nullptr) { }
		circularbuf(size_t max) : circularbuf(max, nullptr) { }

		// this allows the class to be an interface wrapper over a fixed external buffer.
		circularbuf(size_t max, T* backing_store)
		{
			this->cap = max;
			this->ptr = backing_store;

			this->readIdx = 0;
			this->writeIdx = 0;

			if(!this->ptr)  this->resize(max);
			else            this->externalMem = true;
		}

		~circularbuf()
		{
			if(this->ptr && !this->externalMem)
				allocator::deallocate(this->ptr);
		}

		circularbuf(const circularbuf& other)
		{
			this->resize(other.cap);

			this->readIdx = other.readIdx;
			this->writeIdx = other.writeIdx;
		}

		// move
		circularbuf(circularbuf&& other)
		{
			this->cap = other.cap; other.cap = 0;
			this->ptr = other.ptr; other.ptr = 0;
			this->externalMem = other.externalMem; other.externalMem = false;

			this->readIdx = other.readIdx; other.readIdx = 0;
			this->writeIdx = other.writeIdx; other.writeIdx = 0;
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
				if(this->ptr && !this->externalMem)
					allocator::deallocate(this->ptr);

				this->cap = other.cap; other.cap = 0;
				this->ptr = other.ptr; other.ptr = 0;
				this->externalMem = other.externalMem; other.externalMem = false;

				this->readIdx = other.readIdx; other.readIdx = 0;
				this->writeIdx = other.writeIdx; other.writeIdx = 0;
			}

			return *this;
		}

		void resize(size_t newmax)
		{
			if((newmax & (newmax - 1)) != 0)
				aborter::abort("size must be a power of two!");

			auto newptr = (T*) allocator::allocate(sizeof(T) * newmax, alignof(T));

			// cache it, because size() changes on read()
			auto sz = this->size();

			size_t wp = 0;
			for(size_t i = 0; i < sz; i++)
				newptr[wp++ & (newmax - 1)] = this->read();

			if(this->ptr && !this->externalMem)
				allocator::deallocate(this->ptr);

			this->ptr = newptr;
			this->cap = newmax;
			this->readIdx = 0;
			this->writeIdx = wp;
		}



		void write(const T& x)
		{
			this->ptr[this->writeIdx++ & (this->cap - 1)] = x;
		}


		T read()
		{
			if(this->empty()) aborter::abort("read(): empty buffer!");

			return this->ptr[this->readIdx++ & (this->cap - 1)];
		}

		T peek()
		{
			if(this->empty()) aborter::abort("peek(): empty buffer!");
			return this->ptr[this->readIdx];
		}

		T front()
		{
			return peek();
		}

		void pop()
		{
			read_atomic(nullptr);
		}

		void write(T* xs, size_t n)
		{
			for(size_t i = 0; i < n; i++)
				this->write(xs[i]);
		}


		void read(T* buf, size_t n)
		{
			if(this->cnt < n)
				aborter::abort("read(...): not enough elements!");

			for(size_t i = 0; i < n; i++)
				buf[i] = this->read();
		}





		void write_atomic(T&& x)
		{
			auto idx = __atomic_fetch_add(&this->writeIdx, 1, __ATOMIC_SEQ_CST);
			__atomic_store_n(&this->ptr[idx & (this->cap - 1)], x, __ATOMIC_SEQ_CST);
		}

		void write_atomic(const T* x)
		{
			auto idx = __atomic_fetch_add(&this->writeIdx, 1, __ATOMIC_SEQ_CST);
			__atomic_store(&this->ptr[idx & (this->cap - 1)], x, __ATOMIC_SEQ_CST);
		}

		void read_atomic(T* x)
		{
			if(this->empty())
				aborter::abort("read(): empty buffer!");

			auto idx = __atomic_fetch_add(&this->readIdx, 1, __ATOMIC_SEQ_CST);

			if(x) __atomic_store(&this->ptr[idx & (this->cap - 1)], x, __ATOMIC_SEQ_CST);
		}

		void write_atomic(T* xs, size_t n)
		{
			for(size_t i = 0; i < n; i++)
				this->write_atomic(xs[i]);
		}

		void read_atomic(T* buf, size_t n)
		{
			if(this->cnt < n)
				aborter::abort("read(...): not enough elements!");

			if(!buf)
			{
				__atomic_add_fetch(&this->readIdx, n, __ATOMIC_SEQ_CST);
			}
			else
			{
				for(size_t i = 0; i < n; i++)
					this->read_atomic(&buf[i]);
			}
		}



		void clear()
		{
			this->readIdx = 0;
			this->writeIdx = 0;
		}


		iterator begin() { return iterator(this->ptr); }
		iterator end()   { return iterator(this->ptr + this->size()); }

		const_iterator begin() const    { return const_iterator(this->ptr); }
		const_iterator end() const      { return const_iterator(this->ptr + this->size()); }

		const_iterator cbegin() const   { return this->begin(); }
		const_iterator cend() const     { return this->end(); }

		T* data()                                           { return this->ptr; }
		const T* data() const                               { return (const T*) this->data; }
		bool empty() const                                  { return this->size() == 0; }
		size_t size() const                                 { return this->writeIdx - this->readIdx; }
		size_t capacity() const                             { return this->cap; }


		private:
		T* ptr = 0;
		bool externalMem = false;

		size_t readIdx = 0;
		size_t writeIdx = 0;
		size_t cap = 0;
	};
}




