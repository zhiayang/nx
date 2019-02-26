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
			Node(const ElmTy& v) : value(v) { }

			template <typename... Args>
			Node(Args&&... args) : value(krt::forward<Args>(args)...) { }

			ElmTy value;

			Node* prev = 0;
			Node* next = 0;
		};

		struct iterator
		{
			friend listlike_impl;

			iterator& operator ++ ()
			{
				if(!this->node)
					aborter::abort("out of range!");

				this->node = this->node->next;
				return *this;
			}

			iterator& operator -- ()
			{
				if(!this->node)
					aborter::abort("out of range!");

				this->node = this->node->prev;
				return *this;
			}

			iterator operator ++ (int)      { iterator copy(*this); ++(*this); return copy; }
			iterator operator -- (int)      { iterator copy(*this); --(*this); return copy; }


			bool operator == (const iterator& other) const { return other.node == this->node; }
			bool operator != (const iterator& other) const { return other.node != this->node; }

			ElmTy& operator * ()             { return this->node->value; }
			const ElmTy& operator * () const { return this->node->value; }

			ElmTy* operator -> () { return &this->node->value; }
			const ElmTy* operator -> () const { return &this->node->value; }

			iterator(const iterator& other) : node(other.node), self(other.self) { }
			iterator(Node* n, Container* c) : node(n), self(c) { }

			private:
			Node* node;
			Container* self;
		};

		struct const_iterator
		{
			friend listlike_impl;

			const_iterator& operator ++ ()
			{
				if(!this->node)
					aborter::abort("out of range!");

				this->node = this->node->next;
				return *this;
			}

			const_iterator& operator -- ()
			{
				if(!this->node)
					aborter::abort("out of range!");

				this->node = this->node->prev;
				return *this;
			}

			const_iterator operator ++ (int)    { const_iterator copy(*this); ++(*this); return copy; }
			const_iterator operator -- (int)    { iterator copy(*this); --(*this); return copy; }

			bool operator == (const const_iterator& other) const { return other.node == this->node; }
			bool operator != (const const_iterator& other) const { return other.node != this->node; }

			const ElmTy& operator * () const { return this->node->value; }
			const ElmTy* operator -> () const { return &this->node->value; }

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
			Node* dst = 0;
			Node* front = 0;


			while(src)
			{
				auto nd = new (allocator::allocate(sizeof(Node), alignof(Node))) Node(src->value);
				if(dst)
				{
					dst->next = nd;
					nd->prev = dst;
				}
				else
				{
					front = dst;
				}

				dst = nd;
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

		static Node* deleteNode(Container* self, Node* node)
		{
			if(!node) return 0;

			self->cnt -= 1;
			if(node == self->head)
			{
				self->head = node->next;

				if(self->head)
					self->head->prev = 0;
			}
			else if(node == self->tail)
			{
				self->tail = node->prev;

				if(self->tail)
					self->tail->next = 0;
			}
			else
			{
				node->prev->next = node->next;

				if(node->next)
					node->next->prev = node->prev;
			}

			// ok now get rid of the node.
			auto ret = node->next;

			node->~Node();
			allocator::deallocate(node);

			return ret;
		}

		static ElmTy& _insertBefore(Container* self, Node* at, Node* node)
		{
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

			return node->value;
		}

		static ElmTy& insertBefore(Container* self, Node* at, const ElmTy& val)
		{
			auto node = new (allocator::allocate(sizeof(Node), alignof(Node))) Node(val);
			return _insertBefore(self, at, node);
		}

		template <typename... Args>
		static ElmTy& emplaceBefore(Container* self, Node* at, Args&&... args)
		{
			auto node = new (allocator::allocate(sizeof(Node), alignof(Node))) Node(krt::forward<Args>(args)...);
			return _insertBefore(self, at, node);
		}


		static ElmTy& _insertAfter(Container* self, Node* at, Node* node)
		{
			if(at == nullptr) aborter::abort("insertAfter(): cannot insert after end!");

			auto next = at->next;

			node->next = next;
			node->prev = at;
			at->next = node;

			if(next) next->prev = node;
			if(at == self->tail) self->tail = node;

			self->cnt += 1;

			return node->value;
		}

		static ElmTy& insertAfter(Container* self, Node* at, const ElmTy& val)
		{
			auto node = new (allocator::allocate(sizeof(Node), alignof(Node))) Node(val);
			return _insertAfter(self, at, node);
		}

		template <typename... Args>
		static ElmTy& emplaceAfter(Container* self, Node* at, Args&&... args)
		{
			auto node = new (allocator::allocate(sizeof(Node), alignof(Node))) Node(krt::forward<Args>(args)...);
			return _insertAfter(self, at, node);
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













