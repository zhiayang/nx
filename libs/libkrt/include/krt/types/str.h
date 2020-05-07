// str.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"
#include "string.h"

#include "krt/types/impl/arraylike.h"

namespace krt
{
	template <typename allocator, typename aborter, size_t sso_buf_size = 24>
	struct string
	{
		using impl = arraylike_impl<string, char, allocator, aborter, sso_buf_size>;
		friend impl;

		using iterator = ptr_iterator<char>;
		using const_iterator = const_ptr_iterator<char>;

		string() : string("", 0) { }
		string(const char s[]) : string((char*) s, strlen(s)) { }
		string(const char* s, size_t l) : string((char*) s, l) { }

		string(char* s, size_t l)
		{
			this->cnt = 0;
			this->cap = sso_buf_size;
			this->ptr = (sso_buf_size ? this->inline_buffer : nullptr);

			if(s && l)
			{
				this->reserve(l);
				this->cnt = l;

				memmove(this->ptr, s, this->cnt);
				this->ptr[this->cnt] = 0;
			}
		}

		~string() { if(this->ptr && this->ptr != this->inline_buffer) allocator::deallocate(this->ptr); }
		string(const string& other) : string(other.ptr, other.cnt) { }


		private:
		static inline void move_contents(string* dst, string* src)
		{
			dst->cnt = src->cnt; src->cnt = 0;
			if(dst->cnt < sso_buf_size)
			{
				// we need to copy it to our own buffer.
				dst->ptr = dst->inline_buffer;
				memmove(dst->ptr, src->ptr, dst->cnt);

				dst->ptr[dst->cnt] = 0;

				src->ptr = src->inline_buffer;
				src->cap = sso_buf_size;
			}
			else
			{
				dst->cap = src->cap; src->cap = sso_buf_size;
				dst->ptr = src->ptr; src->ptr = (sso_buf_size ? src->inline_buffer : nullptr);
			}
		}

		public:


		// move
		string(string&& other)
		{
			move_contents(this, &other);
		}

		// copy assign
		string& operator = (const string& other)
		{
			if(this != &other)  return *this = string(other);
			else                return *this;
		}

		// move assign
		string& operator = (string&& other)
		{
			if(this != &other)
			{
				if(this->ptr && this->ptr != this->inline_buffer) allocator::deallocate(this->ptr);

				move_contents(this, &other);
			}

			return *this;
		}


		size_t find(char c) const
		{
			for(size_t i = 0; i < this->cnt; i++)
				if(this->ptr[i] == c)
					return i;

			return -1;
		}

		size_t find(const char* s) const
		{
			auto l = strlen(s);
			for(size_t i = 0; i + l <= this->cnt; i++)
			{
				if(strncmp((const char*) this->ptr, s, l) == 0)
					return i;
			}

			return -1;
		}

		void reserve(size_t atleast)
		{
			auto oldptr = this->ptr;

			bool dealloc = impl::reserve(this, atleast);
			if(dealloc && oldptr && oldptr != this->inline_buffer)
				allocator::deallocate(oldptr);
		}

		string substring(size_t idx, size_t len = -1) const { return impl::subarray(this, idx, len); }
		void clear()                                        { impl::clear(this); }

		string& append(const char& c)                       { return impl::append_element(this, c); }
		string& append(const string& s)                     { return impl::append(this, s); }
		string& append(string&& s)                          { return impl::append(this, krt::move(s)); }

		string& append(const char* str)                     { return impl::append(this, str, strlen(str)); }

		char& front()                                       { return impl::front(this); }
		char& back()                                        { return impl::back(this); }

		const char& front() const                           { return impl::front_const(this); }
		const char& back() const                            { return impl::back_const(this); }

		void erase_at(size_t idx, size_t num)               { impl::erase_at(this, idx, num); }
		bool operator == (const string& other) const        { return impl::op_cmpeq(this, other); }
		bool operator != (const string& other) const        { return impl::op_cmpneq(this, other); }
		bool operator < (const string& other) const         { return impl::op_cmplt(this, other); }
		bool operator >  (const string& other) const        { return impl::op_cmpgt(this, other); }
		bool operator >= (const string& other) const        { return impl::op_cmpgeq(this, other); }
		bool operator <= (const string& other) const        { return impl::op_cmpleq(this, other); }

		bool operator == (const char* other) const          { return impl::op_cmpeq(this, other, strlen(other)); }
		bool operator != (const char* other) const          { return impl::op_cmpneq(this, other, strlen(other)); }
		bool operator < (const char* other) const           { return impl::op_cmplt(this, other, strlen(other)); }
		bool operator >  (const char* other) const          { return impl::op_cmpgt(this, other, strlen(other)); }
		bool operator >= (const char* other) const          { return impl::op_cmpgeq(this, other, strlen(other)); }
		bool operator <= (const char* other) const          { return impl::op_cmpleq(this, other, strlen(other)); }

		char& operator [] (size_t idx)                      { return impl::op_subscript(this, idx); }
		const char& operator [] (size_t idx) const          { return impl::op_subscript_const(this, idx); }

		string& operator += (const string& other)           { this->append(other); return *this; }
		string& operator += (string&& other)                { this->append(move(other)); return *this; }
		string& operator += (const char* other)             { this->append(other); return *this; }

		string operator + (const string& other) const       { auto copy = *this; copy.append(other); return copy; }
		string operator + (const char* other) const         { auto copy = *this; copy.append(other); return copy; }

		string& operator += (char other)                    { this->append(string(&other, 1)); return *this; }
		string operator + (char other) const                { auto copy = *this; copy.append(string(&other, 1)); return copy; }


		iterator begin() { return iterator(this->ptr); }
		iterator end()   { return iterator(this->ptr + this->cnt); }

		const_iterator begin() const    { return const_iterator(this->ptr); }
		const_iterator end() const      { return const_iterator(this->ptr + this->cnt); }

		const_iterator cbegin() const   { return this->begin(); }
		const_iterator cend() const     { return this->end(); }


		char* data() const                                  { return this->ptr; }
		size_t size() const                                 { return this->cnt; }
		size_t capacity() const                             { return this->cap; }
		bool empty() const                                  { return this->cnt == 0; }
		char* cstr() const                                  { return this->ptr; }


	private:
		char* ptr = 0;
		size_t cnt = 0;
		size_t cap = 0;

		char inline_buffer[sso_buf_size] = { };

		char* get_pointer() const { return this->ptr; }
	};

	template <typename al, typename ab>
	string<al, ab> operator + (const string<al, ab>& a, const char* b) { return a + string<al, ab>(b); }

	template <typename al, typename ab>
	string<al, ab> operator + (const char* a, const string<al, ab>& b) { return string<al, ab>(a) + b; }
}























