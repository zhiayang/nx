// treemap.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

#include "krt/types/impl/treelike.h"

namespace krt
{
	template <typename K, typename V, typename allocator, typename aborter>
	struct treemap
	{
		using impl = treelike_impl<treemap, K, V, allocator, aborter>;
		using node = typename impl::Node;
		using iterator = typename impl::iterator;
		using const_iterator = typename impl::const_iterator;

		friend impl;

		~treemap() { impl::destroyNode(this->root); }

		treemap() : root(nullptr), cnt(0) { }

		treemap(node* r, size_t sz)
		{
			this->root = impl::copyNode(r);
			this->cnt = sz;
		}

		treemap(const treemap& other) : treemap(other.root, other.cnt) { }

		// move
		treemap(treemap&& other)
		{
			this->root = other.root; other.root = 0;
			this->cnt = other.cnt; other.cnt = 0;
		}

		// copy assign
		treemap& operator = (const treemap& other)
		{
			if(this != &other)  return *this = treemap(other);
			else                return *this;
		}

		// move assign
		treemap& operator = (treemap&& other)
		{
			if(this != &other)
			{
				impl::destroyNode(this->root);

				this->root = other.root; other.root = 0;
				this->cnt = other.cnt; other.cnt = 0;
			}

			return *this;
		}

		void clear()    { this->cnt = 0; impl::destroyNode(this->root); }
		size_t size()   { return this->cnt; }
		bool empty()    { return this->cnt == 0; }


		V& operator [] (const K& k)
		{
			auto it = this->find(k);

			if(it == this->end())
				this->insert(k, V());

			return this->find(k)->value;
		}

		bool insert(const K& k, const V& v)     { return impl::insert(this, k, v); }
		bool remove(const K& k)                 { return impl::remove(this, k); }

		void erase(const iterator& it)
		{
			if(it != this->end())
				impl::deleteNode(this, it->node);
		}

		iterator find(const K& k)               { auto n = impl::find(this, k); return iterator(n, this); }
		const_iterator find(const K& k) const   { auto n = impl::find(this, k); return const_iterator(n, this); }


		iterator begin()                { auto n = this->root; while(n->left) n = n->left; return iterator(n, this); }
		const_iterator begin() const    { auto n = this->root; while(n->left) n = n->left; return const_iterator(n, this); }
		const_iterator cbegin() const   { return this->begin(); }

		iterator end()                  { return iterator(nullptr, this); }
		const_iterator end() const      { return const_iterator(nullptr, this); }
		const_iterator cend() const     { return this->end(); }

		private:

		node* root = 0;
		size_t cnt = 0;
	};
}


#if 0
// haphazardly taken from http://sidsen.azurewebsites.net/papers/rb-trees-talg.pdf

public class map<K, V>
{
	struct node
	{
	}

	var root: &node
	var size: int

	init()
	{
		root = null
		size = 0
	}




}
#endif