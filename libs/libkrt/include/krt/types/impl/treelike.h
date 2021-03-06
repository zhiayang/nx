// treelike.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "../../meta.h"

namespace krt
{
	template <typename Container, typename KeyTy, typename ValueTy, typename allocator, typename aborter>
	struct treelike_impl
	{
		struct Node
		{
			friend struct treelike_impl;

			KeyTy key;
			ValueTy value;


			Node* left;
			Node* right;
			Node* parent;

		private:
			long height;
			long balance;

			Node(const KeyTy& k, const ValueTy& v, Node* par) : key(k), value(v),
				left(nullptr), right(nullptr), parent(par), height(0), balance(0)
			{
			}

			Node(const KeyTy& k, ValueTy&& v, Node* par) : key(k), value(move(v)),
				left(nullptr), right(nullptr), parent(par), height(0), balance(0)
			{
			}

			Node(KeyTy&& k, const ValueTy& v, Node* par) : key(move(k)), value(v),
				left(nullptr), right(nullptr), parent(par), height(0), balance(0)
			{
			}

			Node(KeyTy&& k, ValueTy&& v, Node* par) : key(move(k)), value(move(v)),
				left(nullptr), right(nullptr), parent(par), height(0), balance(0)
			{
			}

			~Node()
			{
				if(this->left)
				{
					this->left->~Node();
					allocator::deallocate((void*) this->left);
				}

				if(this->right)
				{
					this->right->~Node();
					allocator::deallocate((void*) this->right);
				}
			}
		};

		struct iterator
		{
			iterator& operator ++ ()        { this->node = successor(self, node); return *this; }
			iterator operator ++ (int)      { iterator copy(*this); ++(*this); return copy; }

			bool operator == (const iterator& other) const { return other.node == this->node; }
			bool operator != (const iterator& other) const { return other.node != this->node; }

			auto operator * ()
			{
				return krt::pair<KeyTy&, ValueTy&>(node->key, node->value);
			}

			const auto operator * () const
			{
				return krt::pair<const KeyTy&, const ValueTy&>(node->key, node->value);
			}

			Node* operator -> () { return node; }

			iterator(const iterator& other) : node(other.node), self(other.self) { }
			iterator(Node* n, Container* c) : node(n), self(c) { }

			iterator& operator = (const iterator& other)
			{
				this->node = other.node;
				this->self = other.self;
				return *this;
			}

			private:
			Node* node;
			Container* self;
		};

		struct const_iterator
		{
			const_iterator& operator ++ ()      { this->node = successor(self, node); return *this; }
			const_iterator operator ++ (int)    { const_iterator copy(*this); ++(*this); return copy; }

			bool operator == (const const_iterator& other) const { return other.node == this->node; }
			bool operator != (const const_iterator& other) const { return other.node != this->node; }

			const auto operator * () const
			{
				return krt::pair<const KeyTy&, const ValueTy&>(node->key, node->value);
			}

			const Node* operator -> () const { return (const Node*) node; }

			const_iterator(const const_iterator& other) : node(other.node), self(other.self) { }
			const_iterator(Node* n, const Container* c) : node(n), self(c) { }

			const_iterator& operator = (const const_iterator& other)
			{
				this->node = other.node;
				this->self = other.self;
				return *this;
			}

			private:
			Node* node;
			const Container* self;
		};

		template <typename E = bool>
		static Node* copyNode(Container* self, Node* n)
		{
			if(!n) return nullptr;

			static_assert(::std::is_copy_constructible<KeyTy>::value, "keys are not copyable");
			static_assert(::std::is_copy_constructible<ValueTy>::value, "values are not copyable");

			auto ret = createNode(n->key, n->value, n->parent);
			ret->left = copyNode<E>(self, n->left);
			ret->right = copyNode<E>(self, n->right);

			return ret;
		}








		static Node* getNode(const iterator& it)        { return it.node; }
		static Node* getNode(const const_iterator& it)  { return it.node; }

		static void destroyNode(Node* n)
		{
			if(n)
			{
				n->~Node();
				allocator::deallocate((void*) n);
			}
		}

		static Node* createNode(const KeyTy& k, const ValueTy& v, Node* p)
		{
			return new (allocator::template allocate<Node>(1)) Node(k, v, p);
		}

		static Node* createNode(const KeyTy& k, ValueTy&& v, Node* p)
		{
			return new (allocator::template allocate<Node>(1)) Node(k, move(v), p);
		}

		static void rebalance(Container* self, Node* n)
		{
			if(!n)
				return;

			setBalance(n);

			if(n->balance == -2)
			{
				if(height(n->left->left) >= height(n->left->right))
					n = rotateRight(n);

				else
					n = rotateLeftThenRight(n);
			}
			else if(n->balance == 2)
			{
				if(height(n->right->right) >= height(n->right->left))
					n = rotateLeft(n);

				else
					n = rotateRightThenLeft(n);
			}

			if(n->parent != nullptr) rebalance(self, n->parent);
			else                     self->root = n;
		}


		static Node* rotateLeft(Node* a)
		{
			Node* b = a->right;
			b->parent = a->parent;
			a->right = b->left;

			if(a->right != nullptr)
				a->right->parent = a;

			b->left = a;
			a->parent = b;

			if(b->parent != nullptr)
			{
				if (b->parent->right == a)  b->parent->right = b;
				else                        b->parent->left = b;
			}

			setBalance(a);
			setBalance(b);
			return b;
		}

		static Node* rotateRight(Node* a)
		{
			Node* b = a->left;
			b->parent = a->parent;
			a->left = b->right;

			if(a->left != nullptr)
				a->left->parent = a;

			b->right = a;
			a->parent = b;

			if(b->parent != nullptr)
			{
				if(b->parent->right == a)   b->parent->right = b;
				else                        b->parent->left = b;
			}

			setBalance(a);
			setBalance(b);
			return b;
		}

		static Node* rotateLeftThenRight(Node* n)
		{
			n->left = rotateLeft(n->left);
			return rotateRight(n);
		}

		static Node* rotateRightThenLeft(Node* n)
		{
			n->right = rotateRight(n->right);
			return rotateLeft(n);
		}


		static long height(Node* n)
		{
			if(n == nullptr)
				return -1;

			return n->height;
		}

		static void setHeight(Node* n)
		{
			if(n != nullptr)
				n->height = 1 + __max(height(n->left), height(n->right));
		}

		static void setBalance(Node* n)
		{
			setHeight(n);
			n->balance = height(n->right) - height(n->left);
		}

		static Node* successor(const Container* self, Node* n)
		{
			if(!n) return nullptr;

			if(n->right)
			{
				auto succ = n->right;
				while(succ->left)
					succ = succ->left;

				return succ;
			}
			else if(n->parent)
			{
				if(n == n->parent->left)
				{
					return n->parent;
				}
				else
				{
					auto succ = n;
					while(succ->parent)
					{
						if(succ == succ->parent->right)
							succ = succ->parent;

						else
							return succ->parent;
					}

					return nullptr;
				}
			}
			else
			{
				return nullptr;
			}
		}

		static Node* find(Container* self, const KeyTy& key)
		{
			auto n = self->root;
			while(n)
			{
				if(n->key == key)
					return n;

				if(key < n->key)    n = n->left;
				else                n = n->right;
			}

			return nullptr;
		}

		static Node* findOrInsertDefault(Container* self, const KeyTy& key)
		{
			if(self->root == nullptr)
			{
				self->root = createNode(key, ValueTy(), nullptr);
				self->cnt++;
				return self->root;
			}
			else
			{
				Node* n = self->root;
				Node* p = 0;

				do {
					p = n;

					if(n->key == key)
						return n;

					if(key < n->key)    n = n->left;
					else                n = n->right;

				} while(n);

				// ok we got here, it's nothing.
				if(key < p->key)
				{
					p->left = createNode(key, ValueTy(), p);
					self->cnt++;

					rebalance(self, p);
					return p->left;
				}
				else
				{
					p->right = createNode(key, ValueTy(), p);
					self->cnt++;

					rebalance(self, p);
					return p->right;
				}
			}
		}

		static bool insert(Container* self, const KeyTy& key, ValueTy&& val)
		{
			if(self->root == nullptr)
			{
				self->root = createNode(key, move(val), nullptr);
			}
			else
			{
				Node* n = self->root;

				while(true)
				{
					if(n->key == key)
						return false;

					auto parent = n;

					bool goLeft = key < n->key;
					n = goLeft ? n->left : n->right;

					if(n == nullptr)
					{
						auto newnode = createNode(key, move(val), parent);

						if(goLeft)  parent->left = newnode;
						else        parent->right = newnode;

						rebalance(self, parent);
						break;
					}
				}
			}

			self->cnt += 1;
			return true;
		}

		static bool insert(Container* self, const KeyTy& key, const ValueTy& val)
		{
			if(self->root == nullptr)
			{
				self->root = createNode(key, val, nullptr);
			}
			else
			{
				Node* n = self->root;

				while(true)
				{
					if(n->key == key)
						return false;

					auto parent = n;

					bool goLeft = key < n->key;
					n = goLeft ? n->left : n->right;

					if(n == nullptr)
					{
						auto newnode = createNode(key, val, parent);

						if(goLeft)  parent->left = newnode;
						else        parent->right = newnode;

						rebalance(self, parent);
						break;
					}
				}
			}

			self->cnt += 1;
			return true;
		}

		static void deleteNode(Container* self, Node* node)
		{
			if(!node)
				return;

			if(node->left == nullptr && node->right == nullptr)
			{
				self->cnt -= 1;
				if(node->parent == nullptr)
				{
					destroyNode(self->root), self->root = nullptr;
				}
				else
				{
					auto parent = node->parent;
					if(parent->left && parent->left == node)
						destroyNode(parent->left), parent->left = nullptr;

					else if(parent->right)
						destroyNode(parent->right), parent->right = nullptr;

					rebalance(self, parent->parent);
				}
			}
			else if(node->left != nullptr)
			{
				auto child = node->left;
				while(child->right != nullptr)
					child = child->right;

				node->key = krt::move(child->key);
				node->value = krt::move(child->value);

				deleteNode(self, child);
			}
			else
			{
				auto child = node->right;
				while(child->left != nullptr)
					child = child->left;

				node->key = krt::move(child->key);
				node->value = krt::move(child->value);

				deleteNode(self, child);
			}
		}

		static bool remove(Container* self, const KeyTy& key)
		{
			if(self->root == nullptr)
				return false;

			auto child = self->root;

			while(child != nullptr)
			{
				auto node = child;
				child = (key < node->key ? node->left : node->right);

				if(key == node->key)
				{
					deleteNode(self, node);
					return true;
				}
			}

			return false;
		}
	};
}



















