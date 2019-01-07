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
	template <typename allocator, typename aborter>
	struct string
	{
		using impl = arraylike_impl<string, char, allocator, aborter>;
		friend impl;

		string() : string("", 0) { }
		string(const char s[]) : string((char*) s, strlen(s)) { }
		string(const char* s, size_t l) : string((char*) s, l) { }

		string(char* s, size_t l)
		{
			this->ptr = 0;
			this->cnt = 0;
			this->cap = 0;

			if(s && l)
			{
				this->reserve(l);
				this->cnt = l;

				memmove(this->ptr, s, this->cnt);
				this->ptr[this->cnt] = 0;
			}
		}

		~string() { if(this->ptr) allocator::deallocate(this->ptr); }
		string(const string& other) : string(other.ptr, other.cnt) { }

		// move
		string(string&& other)
		{
			this->ptr = other.ptr; other.ptr = 0;
			this->cnt = other.cnt; other.cnt = 0;
			this->cap = other.cap; other.cap = 0;
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
				if(this->ptr) allocator::deallocate(this->ptr);

				this->ptr = other.ptr; other.ptr = 0;
				this->cnt = other.cnt; other.cnt = 0;
				this->cap = other.cap; other.cap = 0;
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

		string substring(size_t idx, size_t len = -1) const { return impl::subarray(this, idx, len); }
		void reserve(size_t atleast)                        { impl::reserve(this, atleast); }
		void clear()                                        { impl::clear(this); }

		string& append(const char& c)                       { return impl::append_element(this, c); }
		string& append(const string& s)                     { return impl::append(this, s); }

		char& front()                                       { return impl::front(this); }
		char& back()                                        { return impl::back(this); }

		const char& front() const                           { return impl::front_const(this); }
		const char& back() const                            { return impl::back_const(this); }

		void erase_at(size_t idx, size_t num)               { impl::erase_at(this, idx, num); }
		bool operator == (const string& other) const        { return impl::op_cmpeq(this, other); }
		bool operator < (const string& other) const         { return impl::op_cmplt(this, other); }

		bool operator >  (const string& other) const        { return impl::op_cmpgt(this, other); }
		bool operator != (const string& other) const        { return impl::op_cmpneq(this, other); }
		bool operator >= (const string& other) const        { return impl::op_cmpgeq(this, other); }
		bool operator <= (const string& other) const        { return impl::op_cmpleq(this, other); }


		char& operator [] (size_t idx)                      { return impl::op_subscript(this, idx); }
		const char& operator [] (size_t idx) const          { return impl::op_subscript_const(this, idx); }

		string& operator += (const string& other)           { this->append(other); return *this; }
		string operator + (const string& other) const       { auto copy = *this; copy.append(other); return copy; }

		string& operator += (char other)                    { this->append(string(&other, 1)); return *this; }
		string operator + (char other) const                { auto copy = *this; copy.append(string(&other, 1)); return copy; }

		using iterator = ptr_iterator<char>;
		iterator begin() { return iterator(this->ptr); }
		iterator end()   { return iterator(this->ptr + this->cnt); }


		char* data()                                        { return this->ptr; }
		size_t size() const                                 { return this->cnt; }
		size_t capacity() const                             { return this->cap; }
		bool empty() const                                  { return this->cnt == 0; }
		char* cstr() const                                  { return this->ptr; }


		private:
		char* ptr = 0;
		size_t cnt = 0;
		size_t cap = 0;
	};

	template <typename al, typename ab>
	string<al, ab> operator + (const string<al, ab>& a, const char* b) { return a + string<al, ab>(b); }

	template <typename al, typename ab>
	string<al, ab> operator + (const char* a, const string<al, ab>& b) { return string<al, ab>(a) + b; }
}























