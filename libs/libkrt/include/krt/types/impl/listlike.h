// listlike.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"
#include "string.h"

#include "krt/meta.h"
#include "krt/macros.h"
#include "krt/iterators.h"

namespace krt
{
	template <typename Container, typename ElmTy, typename allocator, typename aborter>
	struct listlike_impl
	{
		struct Node
		{
			ElmTy value;

			Node* prev = 0;
			Node* next = 0;
		};

		struct iterator
		{
			friend listlike_impl;

			iterator& operator ++ ()        { this->node = this->node->next; return *this; }
			iterator operator ++ (int)      { iterator copy(*this); ++(*this); return copy; }

			iterator& operator -- ()        { this->node = this->node->prev; return *this; }
			iterator operator -- (int)      { iterator copy(*this); --(*this); return copy; }


			bool operator == (const iterator& other) const { return other.node == this->node; }
			bool operator != (const iterator& other) const { return other.node != this->node; }

			Node& operator * ()             { return this->node->value; }
			const Node& operator * () const { return this->node->value; }

			Node* operator -> () { return this->node; }
			const Node* operator -> () const { return this->node; }

			iterator(const iterator& other) : node(other.node), self(other.self) { }
			iterator(Node* n, Container* c) : node(n), self(c) { }

			private:
			Node* node;
			Container* self;
		};

		struct const_iterator
		{
			friend listlike_impl;

			const_iterator& operator ++ ()      { this->node = this->node->next; return *this; }
			const_iterator operator ++ (int)    { const_iterator copy(*this); ++(*this); return copy; }

			const_iterator& operator -- ()      { this->node = this->node->prev; return *this; }
			const_iterator operator -- (int)    { iterator copy(*this); --(*this); return copy; }

			bool operator == (const const_iterator& other) const { return other.node == this->node; }
			bool operator != (const const_iterator& other) const { return other.node != this->node; }

			const Node& operator * () const { return this->node->value; }
			const Node* operator -> () const { return (const Node*) this->node; }

			const_iterator(const const_iterator& other) : node(other.node), self(other.self) { }
			const_iterator(Node* n, Container* c) : node(n), self(c) { }

			private:
			Node* node;
			Container* self;
		};




		static Node* getNode(const iterator& it)        { return it.node; }
		static Node* getNode(const const_iterator& it)  { return it.node; }


		static ElmTy& front(Container* self)
		{
			if(self->cnt == 0) aborter::abort("front(): empty list");
			return self->head->value;
		}

		static const ElmTy& front_const(const Container* self)
		{
			if(self->cnt == 0) aborter::abort("front(): empty list");
			return self->head->value;
		}

		static ElmTy& back(Container* self)
		{
			if(self->cnt == 0) aborter::abort("back(): empty list");
			return self->tail->value;
		}

		static const ElmTy& back_const(const Container* self)
		{
			if(self->cnt == 0) aborter::abort("back(): empty list");
			return self->tail->value;
		}

		static void clone(Container* self, const Container& other)
		{
			auto src = other.head;
			auto dst = new (allocator::allocate(sizeof(Node), alignof(Node))) Node;
			auto front = dst;

			while(src)
			{
				dst->value = src->value;

				auto nn = new (allocator::allocate(sizeof(Node), alignof(Node))) Node;
				nn->prev = dst;
				dst->next = nn;

				dst = nn;

				src = src->next;
			}

			self->head = front;
			self->tail = dst;
			self->cnt = other.cnt;
		}

		static ElmTy popFront(Container* self)
		{
			if(self->cnt == 0) aborter::abort("popFront(): empty list");

			auto ret = self->head->value;
			auto old = self->head;

			self->head = self->head->next;

			if(self->head)
			{
				self->head->prev = 0;
				if(!self->head->next) self->tail = self->head;
			}
			else
			{
				self->tail = 0;
			}

			// aborter::debuglog("head: %p, next: %p", self->head, self->head->next);

			old->~Node();
			allocator::deallocate(old);

			self->cnt -= 1;
			return ret;
		}

		static ElmTy popBack(Container* self)
		{
			if(self->cnt == 0) aborter::abort("popBack(): empty list");

			auto ret = self->tail->value;
			auto old = self->tail;

			self->tail = self->tail->prev;

			if(self->tail)
			{
				self->tail->next = 0;
				if(!self->tail->prev) self->head = self->tail;
			}
			else
			{
				self->head = 0;
			}

			old->~Node();
			allocator::deallocate(old);

			self->cnt -= 1;
			return ret;
		}


		static void clear(Container* self)
		{
			auto n = self->head;
			while(n)
			{
				auto next = n->next;

				n->~Node();
				allocator::deallocate(n);

				n = next;
			}

			self->cnt = 0;
		}

		void deleteNode(Container* self, Node* node)
		{
			if(!node) return;

			self->cnt -= 1;
			if(node == self->head)
			{
				self->head = node->next;
				self->head->prev = 0;
			}
			else if(node == self->tail)
			{
				self->tail = node->prev;
				self->tail->next = 0;
			}
			else
			{
				node->prev->next = node->next;

				if(node->next)
					node->next->prev = node->prev;
			}
		}


		static void insertBefore(Container* self, Node* at, const ElmTy& val)
		{
			auto node = new (allocator::allocate(sizeof(Node), alignof(Node))) Node;
			node->value = val;

			// insert at the back.
			if(at == nullptr)
			{
				if(self->head)
				{
					node->prev = self->tail;
					node->next = nullptr;

					self->tail->next = node;
				}
				else
				{
					self->head = node;
					self->head->next = 0;
					self->head->prev = 0;
				}

				self->tail = node;
			}
			else
			{
				auto prev = at->prev;

				node->prev = prev;
				node->next = at;
				at->prev = node;

				if(prev)
				{
					prev->next = node;
				}
				else
				{
					// we are the head!
					self->head = node;
				}
			}

			self->cnt += 1;
		}

		static void insertAfter(Container* self, Node* at, const ElmTy& val)
		{
			if(at == nullptr) aborter::abort("insertAfter(): cannot insert after end!");

			auto node = new (allocator::allocate(sizeof(Node), alignof(Node))) Node;
			node->value = val;

			auto next = at->next;

			node->next = next;
			node->prev = at;
			at->next = node;

			if(next) next->prev = node;

			if(at == self->tail) self->tail = node;
		}





		static bool op_cmpeq(const Container* self, const Container& other)
		{
			if(self->cnt != other.cnt) return false;
			for(size_t i = 0; i < self->cnt; i++)
			{
				if(self->ptr[i] != other.ptr[i])
					return false;
			}

			return true;
		}

		static bool op_cmplt(const Container* self, const Container& other)
		{
			if(self->cnt != other.cnt) return false;

			// sadly we do not have the min macro.
			for(size_t i = 0; i < (other.cnt < self->cnt ? other.cnt : self->cnt); i++)
			{
				if(self->ptr[i] < other.ptr[i])
					return true;

				else if(self->ptr[i] > other.ptr[i])
					return false;
			}

			// well they were equal -- we return the shorter array.
			return self->cnt < other.cnt;
		}

		static bool op_cmpgt(const Container* self, const Container& other) { return (other < *self); }
		static bool op_cmpneq(const Container* self, const Container& other) { return !(*self == other); }
		static bool op_cmpgeq(const Container* self, const Container& other) { return !(*self < other); }
		static bool op_cmpleq(const Container* self, const Container& other) { return !(other < *self); }
	};
}













