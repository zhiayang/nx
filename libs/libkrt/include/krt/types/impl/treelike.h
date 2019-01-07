// treelike.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

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


			private:
			Node* left;
			Node* right;
			Node* parent;

			long height;
			long balance;

			Node(const KeyTy& k, const ValueTy& v, Node* par) : key(k), value(v),
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
			iterator& operator ++ ()        { self->node = successor(self, node); return *this; }
			iterator operator ++ (int)      { iterator copy(*this); ++(*this); return copy; }

			bool operator == (const iterator& other) const { return other.node == this->node; }
			bool operator != (const iterator& other) const { return other.node != this->node; }

			Node& operator * ()             { return krt::pair(node->key, node->value); }
			const Node& operator * () const { return krt::pair(node->key, node->value); }

			Node* operator -> () { return node; }

			iterator(const iterator& other) : node(other.node), self(other.self) { }
			iterator(Node* n, Container* c) : node(n), self(c) { }

			private:
			Node* node;
			Container* self;
		};

		struct const_iterator
		{
			const_iterator& operator ++ ()      { self->node = successor(self, node); return *this; }
			const_iterator operator ++ (int)    { const_iterator copy(*this); ++(*this); return copy; }

			bool operator == (const const_iterator& other) const { return other.node == this->node; }
			bool operator != (const const_iterator& other) const { return other.node != this->node; }

			const Node& operator * () const { return krt::pair(node->key, node->value); }

			const Node* operator -> () const { return (const Node*) node; }

			const_iterator(const const_iterator& other) : node(other.node), self(other.self) { }
			const_iterator(Node* n, Container* c) : node(n), self(c) { }

			private:
			Node* node;
			Container* self;
		};

		static Node* copyNode(Node* n)
		{
			if(!n) return nullptr;

			auto ret = createNode(n->key, n->value, n->parent);
			ret->left = copyNode(n->left);
			ret->right = copyNode(n->right);;

			return ret;
		}










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
			return new ((void*) allocator::allocate(sizeof(Node), alignof(Node))) Node(k, v, p);
		}

		static void rebalance(Container* self, Node* n)
		{
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

			if(n->parent != NULL)   rebalance(self, n->parent);
			else                    self->root = n;
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

		static Node* successor(Container* self, Node* n)
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
						if(succ = succ->parent->right)
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
			self->cnt -= 1;
			if(node->left == nullptr && node->right == nullptr)
			{
				if(node->parent == nullptr)
				{
					destroyNode(self->root), self->root = nullptr;
				}
				else
				{
					auto parent = node->parent;
					if(parent->left == node)
						destroyNode(parent->left), parent->left = nullptr;

					else
						destroyNode(parent->right), parent->left = nullptr;

					rebalance(self, parent);
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


















