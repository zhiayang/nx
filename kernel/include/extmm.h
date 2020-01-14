// extmm.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "nx.h"

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



	template <typename T = void>
	struct State : _State<T>
	{
		typename _State<T>::extent_t* head = 0;

		size_t numExtents = 0;

		const char* owner = 0;

		addr_t bootstrapStart = 0;
		addr_t bootstrapWatermark = 0;
		addr_t bootstrapEnd = 0;

		mutex lock;


		void dump()
		{
			serial::debugprintf("dumping extmm state %s (%p):\n", this->owner, this);
			auto ext = this->head;
			while(ext)
			{
				serial::debugprintf("    %p - %p\n", ext->addr, end(ext));
				ext = ext->next;
			}
			serial::debugprintf("\n");
		}

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


		addr_t allocate(size_t num, bool (*satisfies)(addr_t, size_t))
		{
			if(num == 0) abort("extmm/%s::allocate(): cannot allocate 0 pages!", this->owner);

			autolock lk(&this->lock);

			auto ext = this->head;
			while(ext)
			{
				if(ext->size >= num && satisfies(ext->addr, ext->size))
				{
					// return this.
					// take from the bottom.

					addr_t ret = ext->addr;
					ext->addr += (num * PAGE_SIZE);
					ext->size -= num;

					return ret;
				}

				ext = ext->next;
			}

			serial::debugprintf("extmm/%s::allocate(): out of pages!\n", this->owner);
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

				ext = ext->next;
			}

			// oops. make a new extent.
			this->addExtent(addr, num);
		}


	private:
		static addr_t end(addr_t base, size_t num)          { return base + (num * PAGE_SIZE); }
		static addr_t end(typename _State<T>::extent_t* ext){ return ext->addr + (ext->size * PAGE_SIZE); }


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

}
}






