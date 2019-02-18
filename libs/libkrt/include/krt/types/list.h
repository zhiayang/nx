// list.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"
#include "string.h"

#include "krt/types/impl/listlike.h"

namespace krt
{
	template <typename T, typename allocator, typename aborter>
	struct list
	{
		using impl = listlike_impl<list, T, allocator, aborter>;
		friend impl;

		using node_t = typename impl::Node;
		using iterator = typename impl::iterator;
		using const_iterator = typename impl::const_iterator;


		list() : head(nullptr), cnt(0) { }
		~list() { if(this->head) allocator::deallocate(this->head); }

		// copy construct
		list(const list& other)
		{
			this->cnt = 0;
			this->head = 0;

			impl::clone(this, other);
		}

		// move construct
		list(list&& other)
		{
			this->head = other.head; other.head = 0;
			this->cnt = other.cnt; other.cnt = 0;
		}

		// copy assign
		list& operator = (const list& other)
		{
			if(this != &other)  return *this = list(other);
			else                return *this;
		}

		// move assign
		list& operator = (list&& other)
		{
			if(this != &other)
			{
				if(this->head) allocator::deallocate(this->head);

				this->head = other.head; other.head = 0;
				this->cnt = other.cnt; other.cnt = 0;
			}

			return *this;
		}

		void clear()                        { impl::clear(this); }

		iterator erase(const iterator& it)  { return iterator(impl::deleteNode(this, impl::getNode(it)), this); }

		void insertAfter(const iterator& it, const T& val)  { impl::insertAfter(this, impl::getNode(it), val); }
		void insertBefore(const iterator& it, const T& val) { impl::insertBefore(this, impl::getNode(it), val); }

		void append(const T& val)       { this->insertBefore(this->end(), val); }
		void prepend(const T& val)      { this->insertBefore(this->begin(), val); }

		T popFront()                    { return impl::popFront(this); }
		T popBack()                     { return impl::popBack(this); }

		T& front()                      { return impl::front(this); }
		const T& front() const          { return impl::front_const(this); }

		T& back()                       { return impl::back(this); }
		const T& back() const           { return impl::back_const(this); }


		// bool operator == (const list& other) const         { return impl::op_cmpeq(this, other); }
		// bool operator < (const list& other) const          { return impl::op_cmplt(this, other); }
		// bool operator >  (const list& other) const         { return impl::op_cmpgt(this, other); }
		// bool operator != (const list& other) const         { return impl::op_cmpneq(this, other); }
		// bool operator >= (const list& other) const         { return impl::op_cmpgeq(this, other); }
		// bool operator <= (const list& other) const         { return impl::op_cmpleq(this, other); }


		iterator begin()                { return iterator(this->head, this); }
		const_iterator begin() const    { return const_iterator(this->head, this); }
		const_iterator cbegin() const   { return this->begin(); }

		iterator end()                  { return iterator(nullptr, this); }
		const_iterator end() const      { return const_iterator(nullptr, this); }
		const_iterator cend() const     { return this->end(); }


		bool empty() const                                  { return this->cnt == 0; }
		size_t size() const                                 { return this->cnt; }

		private:
		node_t* head = 0;
		node_t* tail = 0;
		size_t cnt = 0;
	};
}



















