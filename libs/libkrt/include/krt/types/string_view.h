// string_view.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"
#include "string.h"

#include "krt/types/str.h"
#include "krt/types/impl/arraylike.h"

namespace krt
{
	template <typename allocator, typename aborter>
	struct string_view
	{
		using impl = arraylike_impl<string_view, const char, allocator, aborter>;
		friend impl;

		string_view() : string_view("", 0) { }

		string_view(const char s[]) : string_view((char*) s, strlen(s)) { }
		string_view(const char* s, size_t l) : string_view((char*) s, l) { }

		string_view(char* s, size_t l)
		{
			this->ptr = s;
			this->cnt = l;
		}

		string_view(const string<allocator, aborter>& s)
		{
			this->ptr = (const char*) s.data();
			this->cnt = s.size();
		}

		~string_view() { }
		string_view(const string_view& other) : string_view(other.ptr, other.cnt) { }

		// move
		string_view(string_view&& other)
		{
			this->ptr = other.ptr; other.ptr = 0;
			this->cnt = other.cnt; other.cnt = 0;
		}

		// copy assign
		string_view& operator = (const string_view& other)
		{
			if(this != &other)  return *this = string_view(other);
			else                return *this;
		}

		// move assign
		string_view& operator = (string_view&& other)
		{
			if(this != &other)
			{
				this->ptr = other.ptr; other.ptr = 0;
				this->cnt = other.cnt; other.cnt = 0;
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


		string_view substring(size_t idx, size_t len = -1) const { return impl::subarray(this, idx, len); }
		void clear()                                        { this->ptr = 0; this->cnt = 0; }

		void remove_prefix(size_t n)
		{
			if(n > this->cnt) aborter::abort("remove_prefix(): not enough elements");
			this->ptr += n;
		}

		void remove_suffix(size_t n)
		{
			if(n > this->cnt) aborter::abort("remove_suffix(): not enough elements");
			this->cnt -= n;
		}



		char& front()                                       { return impl::front(this); }
		char& back()                                        { return impl::back(this); }

		const char& front() const                           { return impl::front_const(this); }
		const char& back() const                            { return impl::back_const(this); }

		bool operator == (const string_view& other) const   { return impl::op_cmpeq(this, other); }
		bool operator < (const string_view& other) const    { return impl::op_cmplt(this, other); }

		bool operator >  (const string_view& other) const   { return impl::op_cmpgt(this, other); }
		bool operator != (const string_view& other) const   { return impl::op_cmpneq(this, other); }
		bool operator >= (const string_view& other) const   { return impl::op_cmpgeq(this, other); }
		bool operator <= (const string_view& other) const   { return impl::op_cmpleq(this, other); }

		const char& operator [] (size_t idx) const          { return impl::op_subscript_const(this, idx); }

		using iterator = ptr_iterator<char>;
		iterator begin() { return iterator(this->ptr); }
		iterator end()   { return iterator(this->ptr + this->cnt); }

		const char* data() const                            { return this->ptr; }
		size_t size() const                                 { return this->cnt; }
		bool empty() const                                  { return this->cnt == 0; }

		krt::string<allocator, aborter> str() const         { return string<allocator, aborter>(this->ptr, this->cnt); }

		private:
		const char* ptr = 0;
		size_t cnt = 0;
	};
}























