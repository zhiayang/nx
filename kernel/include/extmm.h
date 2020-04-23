// extmm.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "synchro.h"
#include "devices/serial.h"

#include <type_traits>

namespace nx {
namespace extmm
{
	template <typename ExtraData, typename E = void>
	struct _State;

	template <>
	struct _State<void>
	{
		struct extent_t
		{
			uint64_t addr;
			uint64_t size;

			extent_t* next = 0;
		};
	};

	template <typename ExtraData>
	struct _State<ExtraData, std::enable_if_t<!std::is_void_v<ExtraData>>>
	{
		struct extent_t
		{
			uint64_t addr;
			uint64_t size;

			extent_t* next = 0;

			ExtraData data;
		};
	};



	template <typename T = void, typename Lock = nx::mutex>
	struct State : _State<T>
	{
		using extent_t = typename _State<T>::extent_t;

		extent_t* head = 0;

		size_t numExtents = 0;

		const char* owner = 0;

		addr_t bootstrapStart = 0;
		addr_t bootstrapWatermark = 0;
		addr_t bootstrapEnd = 0;

		Lock lock;

		void init(const char* owner, addr_t base, addr_t top)
		{
			this->head = 0;
			this->numExtents = 0;

			this->owner = owner;

			this->bootstrapWatermark = base;
			this->bootstrapStart = base;
			this->bootstrapEnd = top;
		}

		void destroy()
		{
			autolock lk(&this->lock);

			// just loop through all the extents and destroy them.
			auto ext = this->head;
			while(ext)
			{
				if(!(((addr_t) ext) >= this->bootstrapStart && ((addr_t) ext) < this->bootstrapEnd))
				{
					delete ext;
				}

				// else do nothing.

				ext = ext->next;
			}

			this->head = 0;
			this->numExtents = 0;
			this->bootstrapWatermark = this->bootstrapStart;
		}

		template <typename Predicate>
		addr_t allocate(size_t num, const Predicate& satisfies)
		{
			if(num == 0) abort("extmm/%s::allocate(): cannot allocate 0 pages!", this->owner);

			autolock lk(&this->lock);

			auto ext = this->head;
			extent_t* prev = 0;

			while(ext)
			{
				if(ext->size >= num && satisfies(ext->addr, num))
				{
					// return this.
					// take from the bottom.

					addr_t ret = ext->addr;
					ext->addr += (num * PAGE_SIZE);
					ext->size -= num;

					if(ext->size == 0)
					{
						auto pn = ext->next;

						// delete the extent.
						if(!(((addr_t) ext) >= this->bootstrapStart && ((addr_t) ext) < this->bootstrapEnd))
							delete ext;

						if(prev)    prev->next = pn;
						else        this->head = pn;
					}

					return ret;
				}

				prev = ext;
				ext = ext->next;
			}

			error("extmm", "(%s): allocate() out of pages!", this->owner);
			return 0;
		}

		addr_t allocateSpecific(addr_t start, size_t num)
		{
			autolock lk(&this->lock);

			auto ext = this->head;
			while(ext)
			{
				if(ext->addr <= start && end(ext) >= end(start, num))
				{
					if(ext->addr == start)
					{
						// simple -- just move the pointer up
						ext->addr += (num * PAGE_SIZE);
						ext->size -= num;
					}
					else if(end(start, num) == end(ext))
					{
						// also simple -- just subtract the size.
						ext->size -= num;
					}
					else
					{
						// bollocks, it's somewhere in the middle.
						size_t numFrontPages = (start - ext->addr) / PAGE_SIZE;
						size_t numBackPages = (end(ext) - end(start, num)) / PAGE_SIZE;

						// decrease the front block
						ext->size = numFrontPages;

						// make a new block
						this->addExtent(end(start, num), numBackPages);
					}

					return start;
				}

				ext = ext->next;
			}

			serial::debugprintf("extmm/%s::allocateSpecific(): could not fulfil request!\n", this->owner);
			return 0;
		}

		void deallocate(addr_t addr, size_t num)
		{
			autolock lk(&this->lock);

			// loop through every extent. we can match in two cases:
			// 1. 'addr' is 'num' pages *below* the base of the extent
			// 2. 'addr' is *size* pages above the *end* of the extent

			auto ext = this->head;
			while(ext)
			{
				if(end(addr, num) == ext->addr)
				{
					// we are below. decrease the addr of the extent by num pages.
					ext->addr -= (PAGE_SIZE * num);
					return;
				}
				else if(end(ext) == addr)
				{
					// increase the size only
					ext->size += num;
					return;
				}
				else if(ext->addr == addr)
				{
					// hmm...
					abort("extmm: potential double free! addr: %p, size: %zu", addr, num);
				}

				ext = ext->next;
			}

			// oops. make a new extent.
			this->addExtent(addr, num);
		}


		void dump()
		{
			serial::debugprintf("dumping extmm state %s (%p):\n", this->owner, this);
			auto ext = this->head;
			while(ext)
			{
				serial::debugprintf("    %p - %p (%zu)\n", ext->addr, end(ext), ext->size);
				ext = ext->next;
			}
			serial::debugprintf("\n");
		}

		bool checkConsistency()
		{
			// LOOK, N^2!
			auto ext1 = this->head;
			while(ext1)
			{
				auto ext2 = ext1->next;
				while(ext2)
				{
					// check if they intersect.
					if(intersects(ext1, ext2))
					{
						error("extmm", "inconsistent extent state! (%p, %p, %zu) intersects (%p, %p, %zu)",
							ext1->addr, end(ext1), ext1->size, ext2->addr, end(ext2), ext2->size);

						return false;
					}

					ext2 = ext2->next;
				}

				if(ext1->size == 0)
				{
					error("extmm", "inconsistent extent state! extent at %p has 0 size", ext1->addr);
					return false;
				}

				ext1 = ext1->next;
			}

			return true;
		}

	private:
		static addr_t end(addr_t base, size_t num)          { return base + (num * PAGE_SIZE); }
		static addr_t end(typename _State<T>::extent_t* ext){ return ext->addr + (ext->size * PAGE_SIZE); }

		bool intersects(extent_t* a, extent_t* b)
		{
			if(!a || !b) return false;

			// check if A lies within B, or if B lies within A.
			return (a->addr >= b->addr && end(b) >= a->addr) || (b->addr >= a->addr && end(a) >= b->addr);
		}

		void addExtent(addr_t addr, size_t size)
		{
			using extent_t = typename _State<T>::extent_t;

			extent_t* ext = 0;
			if(this->bootstrapWatermark + sizeof(extent_t) <= this->bootstrapEnd)
			{
				ext = new ((extent_t*) this->bootstrapWatermark) extent_t();
				this->bootstrapWatermark += sizeof(extent_t);
			}
			else
			{
				ext = new extent_t();
			}

			assert(ext);
			ext->addr = addr;
			ext->size = size;

			// insert at the head.
			ext->next = this->head;

			this->head = ext;
			this->numExtents += 1;
		}
	};

	void dump(const State<>& st);
}
}






