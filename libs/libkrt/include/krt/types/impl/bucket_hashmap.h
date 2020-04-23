// bucket_hashmap.h
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"
#include "string.h"

#include "krt/meta.h"
#include "krt/iterators.h"


namespace krt
{
	template <typename KeyTy, typename ValueTy, typename allocator, typename aborter,
		typename Hasher = krt::hash<KeyTy>>
	struct bucket_hashmap
	{
		struct Bucket;

		template <bool IsConst>
		struct iterator_base
		{
			friend struct bucket_hashmap;

			bool operator == (const iterator_base& other) const { return other.node == this->node; }
			bool operator != (const iterator_base& other) const { return other.node != this->node; }

			template <bool c = IsConst, typename E = std::enable_if_t<!c>>
			auto operator * ()
			{
				return krt::pair<const KeyTy&, ValueTy&>(node->key, node->value);
			}

			template <bool c = IsConst, typename E = std::enable_if_t<c>>
			auto operator * () const
			{
				return krt::pair<const KeyTy&, const ValueTy&>(node->key, node->value);
			}

			Bucket* operator -> () { return node; }

			template <bool c = IsConst, typename E = std::enable_if_t<c>>
			iterator_base(Bucket* n, const bucket_hashmap* self, size_t bucket) : node(n), self(self), bucketIdx(bucket) { }

			template <bool c = IsConst, typename E = std::enable_if_t<!c>>
			iterator_base(Bucket* n, bucket_hashmap* self, size_t bucket) : node(n), self(self), bucketIdx(bucket) { }


			iterator_base(const iterator_base& other) : node(other.node), self(other.self), bucketIdx(other.bucketIdx) { }

			iterator_base& operator = (const iterator_base& other)
			{
				this->node = other.node;
				this->self = other.self;
				return *this;
			}

			iterator_base operator ++ (int)      { iterator_base copy(*this); ++(*this); return copy; }
			iterator_base& operator ++ ()
			{
				if(this->node->next)
				{
					this->node = this->node->next;
				}
				else
				{
					this->node = 0;

					while(this->bucketIdx + 1 < self->numBuckets && !this->node)
						this->node = self->ptr[++this->bucketIdx];
				}

				return *this;
			}

		private:
			Bucket* node;
			std::conditional_t<IsConst, const bucket_hashmap*, bucket_hashmap*> self;
			size_t bucketIdx;
		};


		struct Bucket
		{
			friend bucket_hashmap;

			const KeyTy key;
			ValueTy value;

		private:
			Bucket(const KeyTy& k, const ValueTy& v) : key(k), value(v) { }
			Bucket(const KeyTy& k, ValueTy&& v) : key(k), value(move(v)) { }

			template <typename... Args>
			Bucket(const KeyTy& k, Args&&... args) : key(k), value(args...) { }

			Bucket(const Bucket&) = delete;
			Bucket(Bucket&& other)
			{
				this->next = other.next; other.next = 0;

				this->key = move(other.key);
				this->value = move(other.value);
			}

			Bucket* clone()
			{
				auto ret = new (allocator::template allocate<Bucket>(1)) Bucket(this->key, this->value);
				if(this->next) ret->next = this->next->clone();

				return ret;
			}


			Bucket* next = 0;
		};


		using iterator = iterator_base<false>;
		using const_iterator = iterator_base<true>;



		~bucket_hashmap()
		{
			if(this->ptr)
				allocator::deallocate(this->ptr);
		}

		bucket_hashmap(const Hasher& hasher)
		{
			this->hasher = hasher;
		}

		bucket_hashmap()
		{
			this->hasher = Hasher();
		}

		bucket_hashmap(const bucket_hashmap& other)
		{
			this->numBuckets = other.numBuckets;

			this->ptr = allocator::template allocate<Bucket*>(this->numBuckets);
			memset(this->ptr, 0, this->numBuckets * sizeof(Bucket*));

			this->cnt = other.cnt;

			// copy the nodes, with cloning. each node will clone its next pointers as well, recursively.
			for(size_t i = 0; i < this->numBuckets; i++)
				this->ptr[i] = (other.ptr[i] ? other.ptr[i]->clone() : nullptr);
		}

		bucket_hashmap(bucket_hashmap&& other)
		{
			this->ptr = other.ptr;  other.ptr = 0;
			this->cnt = other.cnt;  other.cnt = 0;
			this->numBuckets = other.numBuckets; other.numBuckets = 0;
		}

		bucket_hashmap& operator = (const bucket_hashmap& other)
		{
			if(this != &other)  return *this = bucket_hashmap(other);
			else                return *this;
		}

		bucket_hashmap& operator = (bucket_hashmap&& other)
		{
			if(this != &other)
			{
				if(this->ptr)  allocator::deallocate(this->ptr);

				this->ptr = other.ptr;  other.ptr = 0;
				this->cnt = other.cnt;  other.cnt = 0;
				this->numBuckets = other.numBuckets; other.numBuckets = 0;
			}

			return *this;
		}

		iterator find(const KeyTy& key)
		{
			auto [ n, b ] = __internal_find(key);
			return iterator(n, this, b);
		}

		const_iterator find(const KeyTy& key) const
		{
			auto [ n, b ] = __internal_find(key);
			return const_iterator(n, this, b);
		}

		iterator insert(const KeyTy& key, const ValueTy& value)
		{
			this->ensureSize();
			return __insert_internal(this->ptr, this->numBuckets,
				new (allocator::template allocate<Bucket>(1)) Bucket(key, value));
		}

		iterator insert(const KeyTy& key, ValueTy&& value)
		{
			this->ensureSize();
			return __insert_internal(this->ptr, this->numBuckets,
				new (allocator::template allocate<Bucket>(1)) Bucket(key, move(value)));
		}

		template <typename... Args>
		iterator emplace(const KeyTy& key, Args&&... args)
		{
			this->ensureSize();
			return __insert_internal(this->ptr, this->numBuckets,
				new (allocator::template allocate<Bucket>(1)) Bucket(key, args...));
		}



		bool remove(const KeyTy& key)
		{
			auto it = this->find(key);
			if(it == this->end())
				return false;

			this->erase(it);
			return true;
		}

		void clear()
		{
			for(size_t i = 0; i < this->numBuckets; i++)
			{
				auto node = this->ptr[i];
				while(node)
				{
					auto next = node->next;

					node->~Bucket();
					allocator::deallocate(node);

					node = next;
				}

				this->ptr[i] = nullptr;
			}

			this->cnt = 0;
			this->ensureSize();
		}

		ValueTy& operator [] (const KeyTy& key)
		{
			auto it = this->find(key);
			if(it != this->end())
				return it->value;

			else
				return this->insert(key, ValueTy())->value;
		}




		iterator erase(const iterator& it)
		{
			if(it.self != this)
			{
				aborter::abort("bucket_hashmap::erase(): trying to erase foreign iterator");
			}
			else if(it.node == nullptr || it.bucketIdx >= this->numBuckets)
			{
				aborter::abort("bucket_hashmap::erase(): trying to erase invalid iterator (node = %p, bucket = %zu)",
					it.node, it.bucketIdx);
			}


			auto node = this->ptr[it.bucketIdx];
			if(!node) aborter::abort("bucket_hashmap::erase(): no node at bucket = %zu", it.bucketIdx);

			if(node == it.node)
			{
				// lucky.
				auto next = node->next;
				this->ptr[it.bucketIdx] = next;

				// destruct it.
				node->~Bucket();
				allocator::deallocate(node);

				this->cnt -= 1;

				return iterator(next, this, it.bucketIdx);
			}
			else
			{
				while(node->next != it.node)
					node = node->next;

				if(!node)                   aborter::abort("bucket_hashmap::erase(): no such node");
				if(node->next != it.node)   aborter::abort("bucket_hashmap::erase(): invalid state");

				auto next = it.node->next;

				node->next = next;
				it.node->~Bucket();
				allocator::deallocate(it.node);

				this->cnt -= 1;

				return iterator(next, this, it.bucketIdx);
			}
		}



		const_iterator begin() const    { auto [ n, b ] = __internal_begin(); return const_iterator(n, this, b); }
		const_iterator cbegin() const   { return this->begin(); }

		const_iterator end() const      { return const_iterator(nullptr, this, this->numBuckets); }
		const_iterator cend() const     { return this->end(); }

		iterator begin()                { auto [ n, b ] = __internal_begin(); return iterator(n, this, b); }
		iterator end()                  { return iterator(nullptr, this, this->numBuckets); }

		size_t size() const     { return this->cnt; }
		bool empty() const      { return this->cnt == 0; }
		size_t buckets() const  { return this->numBuckets; }







	private:
		static constexpr size_t STARTING_SIZE = 16;

		// when the load is > 2/3, grow the size.
		static constexpr size_t MAX_LOAD_NUMER = 2;
		static constexpr size_t MAX_LOAD_DENOM = 3;

		// when the load is < 1/5, shrink the size.
		// note that this is not 1/3, because if the size is halved when shrinking,
		// the next insertion will immediately double the table again.
		static constexpr size_t MIN_LOAD_NUMER = 1;
		static constexpr size_t MIN_LOAD_DENOM = 5;

		void ensureSize()
		{
			size_t newcap = this->numBuckets;

			if(this->cnt >= (this->numBuckets * MAX_LOAD_NUMER) / MAX_LOAD_DENOM)
				newcap = nextSize();

			else if(this->numBuckets > STARTING_SIZE && this->cnt <= (this->numBuckets * MIN_LOAD_NUMER) / MIN_LOAD_DENOM)
				newcap = prevSize();

			if(newcap != this->numBuckets)
			{
				auto newptr = allocator::template allocate<Bucket*>(newcap);
				memset(newptr, 0, newcap * sizeof(Bucket*));

				for(auto it = this->begin(); it != this->end();)
				{
					// we need to get the next iterator, because we are going to invalidate the node
					// which will mess with its next pointer!
					auto prev = it++;

					// don't increase the count!
					__insert_internal(newptr, newcap, prev.node, /* incr_count: */ false);
				}

				// get rid of the old array
				allocator::deallocate(this->ptr);

				this->numBuckets = newcap;
				this->ptr = newptr;
			}
		}

		iterator __insert_internal(Bucket** node_array, size_t cap, Bucket* newnode, bool incr_count = true)
		{
			auto idx = indexForKey(newnode->key, cap);
			newnode->next = 0;

			if(auto node = node_array[idx]; node)
			{
				node_array[idx] = newnode;
				newnode->next = node;
			}
			else
			{
				node_array[idx] = newnode;
			}

			if(incr_count)
				this->cnt += 1;

			return iterator(newnode, this, idx);
		}

		krt::pair<Bucket*, size_t> __internal_begin() const
		{
			if(!this->ptr)
				return { nullptr, this->numBuckets };

			for(size_t i = 0; i < this->numBuckets; i++)
			{
				if(this->ptr[i])
					return { this->ptr[i], i };
			}

			return { nullptr, this->numBuckets };
		}

		krt::pair<Bucket*, size_t> __internal_find(const KeyTy& key) const
		{
			if(!this->ptr)
				return { nullptr, this->numBuckets };

			auto idx = indexForKey(key, this->numBuckets);
			auto node = this->ptr[idx];
			while(node)
			{
				if(node->key == key)
					return { node, idx };

				node = node->next;
			}

			return { nullptr, this->numBuckets };
		}

		size_t indexForKey(const KeyTy& key, size_t len) const
		{
			return this->hasher(key) % len;
		}

		size_t nextSize() const
		{
			if(this->numBuckets == 0)
				return STARTING_SIZE;

			return this->numBuckets * 2;
		}

		size_t prevSize() const
		{
			return this->numBuckets / 2;
		}


		Bucket** ptr = 0;
		size_t cnt = 0;
		size_t numBuckets = 0;
		Hasher hasher;
	};
}

