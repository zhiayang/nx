// vmregion.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "mm.h"

#include "krt/types/hashmap.h"

namespace nx {
namespace vmm
{
	static constexpr int SHIFT_AMT = 0;

	static VirtAddr end(VirtAddr addr, size_t num)
	{
		return addr + ofsPages(num);
	}


	void AddressSpace::init(PhysAddr cr3)
	{
		if(cr3.nonZero())   this->cr3 = cr3;
		else                this->cr3 = pmm::allocate(1);
	}

	void AddressSpace::destroy()
	{
		size_t num = 1;
		pmm::deallocate(this->cr3, 1);

		for(auto x : this->allocatedPhysPages)
			num++, pmm::deallocate(x, 1);

		// destroy each region.
		for(const auto& r : this->regions)
		{
			for(auto [ v, p ] : r.backingPhysPages)
			{
				if(p.nonZero())
				{
					num++;
					pmm::deallocate(p, 1);
				}
			}
		}

		log("adrspc", "freed %zu physical pages", num);
	}


	void AddressSpace::addRegion(VirtAddr addr, size_t num)
	{
		assert(heap::initialised());
		assert(addr.isAligned());

		// warn("vmregion", "add (%p, %zu)", addr, num);

		for(auto& vmr : this->regions)
		{
			if(end(addr, num) == vmr.addr)
			{
				vmr.grow_down(num);
				// warn("vmregion", "update: (%p, %zu)", vmr.addr, vmr.numPages);
				return;
			}
			else if(vmr.end() == addr)
			{
				vmr.grow_up(num);
				// warn("vmregion", "update: (%p, %zu)", vmr.addr, vmr.numPages);
				return;
			}
			else if(vmr.addr == addr)
			{
				// hmm...
				abort("vmregion: potential double free! addr: %p, size: %zu", addr, num);
			}
		}

		this->regions.emplace(addr, num);
		// error("vmm", "append %p/%zu", addr, num);
	}

	void AddressSpace::addRegion(VirtAddr addr, PhysAddr physStart, size_t num)
	{
		assert(heap::initialised());
		assert(addr.isAligned() && physStart.isAligned());

		// warn("vmregion", "add(phys) (%p, %zu)", addr, num);

		auto update_phys = [](VMRegion& vmr, VirtAddr virt, PhysAddr phys, size_t num) {
			for(size_t i = 0; i < num; i++)
			{
				// warn("vmregion", "insert %p (%zu / %zu)", virt + (i * PAGE_SIZE),
				// 	vmr.backingPhysPages.size(), vmr.backingPhysPages.buckets());

				vmr.backingPhysPages[virt + ofsPages(i)] = phys + ofsPages(i);
			}
		};

		for(auto& vmr : this->regions)
		{
			if(end(addr, num) == vmr.addr)
			{
				// warn("vmregion", "updating #1");
				vmr.grow_down(num);
				update_phys(vmr, vmr.addr, physStart, num);

				// warn("vmregion", "update: (%p, %zu)", vmr.addr, vmr.numPages);
				return;
			}
			else if(vmr.end() == addr)
			{
				auto oldSz = vmr.numPages;
				// warn("vmregion", "updating #2");

				vmr.grow_up(num);
				update_phys(vmr, vmr.addr + ofsPages(oldSz), physStart, num);

				// warn("vmregion", "update: (%p, %zu)", vmr.addr, vmr.numPages);
				return;
			}
			else if(vmr.addr == addr)
			{
				// hmm...
				abort("vmregion: potential double add! addr: %p, phys: %p, size: %zu/%zu", addr, physStart, num, vmr.numPages);
			}
		}

		// warn("vmregion", "updating #3");

		auto vmr = VMRegion(addr, num);
		update_phys(vmr, vmr.addr, physStart, num);

		this->regions.append(krt::move(vmr));
		// error("vmm", "append %p/%zu", addr, num);
	}

	void AddressSpace::freeRegion(VirtAddr addr, size_t num, bool freePhys)
	{
		assert(heap::initialised());
		assert(addr.isAligned());

		// warn("vmregion", "free (%p, %zu)", addr, num);
		for(auto it = this->regions.begin(); it != this->regions.end(); ++it)
		{
			auto& vmr = *it;

			// check if the region is eligible at all. in theory, the freed region should not
			// be split across two existing regions.
			if(vmr.addr <= addr && vmr.end() >= end(addr, num))
			{
				if(vmr.addr == addr)
				{
					if(freePhys)
					{
						// chop from the front.
						for(size_t i = 0; i < num; i++)
						{
							if(auto p = vmr.backingPhysPages[vmr.addr + ofsPages(i)]; p.nonZero())
								pmm::deallocate(p, 1);
						}
					}

					vmr.shrink_up(num);
					if(vmr.numPages == 0)
						this->regions.erase(it);

					return;
				}
				else if(end(addr, num) == vmr.end())
				{
					if(freePhys)
					{
						// chop from the back.
						for(size_t i = 0; i < num; i++)
						{
							if(auto p = vmr.backingPhysPages[vmr.addr + ofsPages(vmr.numPages - 1 - i)]; p.nonZero())
								pmm::deallocate(p, 1);
						}
					}

					vmr.shrink_down(num);
					if(vmr.numPages == 0)
						this->regions.erase(it);

					return;
				}
				else
				{
					// bollocks, it's somewhere in the middle.
					size_t numFrontPages = (addr - vmr.addr) / PAGE_SIZE;
					size_t numBackPages = (vmr.end() - end(addr, num)) / PAGE_SIZE;

					assert(numFrontPages > 0 && numBackPages > 0);

					auto back_vmr = vmr;

					// clear the physical pages:
					if(freePhys)
					{
						for(size_t i = 0; i < num; i++)
						{
							if(auto p = vmr.backingPhysPages[vmr.addr + ofsPages(numFrontPages + i)]; p.nonZero())
								pmm::deallocate(p, 1);
						}
					}

					// then shrink it *all the way down*:
					vmr.shrink_down(num + numBackPages);
					if(vmr.numPages == 0)
						this->regions.erase(it);

					// shrink_up the back vmr:
					back_vmr.shrink_up(numFrontPages + num);
					if(back_vmr.numPages > 0)
						this->regions.append(back_vmr);

					return;
				}
			}
		}

		abort("vmregion: did not find matching extent for (%p, %zu)", addr, num);
	}

	addr_t AddressSpace::addPhysicalMapping(VirtAddr virt, PhysAddr phys)
	{
		for(auto it = this->regions.begin(); it != this->regions.end(); ++it)
		{
			auto& vmr = *it;

			if(vmr.addr <= virt && virt < vmr.end())
			{
				PhysAddr old;
				if(auto it = vmr.backingPhysPages.find(virt); it != vmr.backingPhysPages.end())
					old = it->value;

				vmr.backingPhysPages[virt] = phys;
				return old.addr();
			}
		}

		abort("vmregion: did not find matching extent for (%p)", virt);
		return 0;
	}




	VMRegion::VMRegion(VirtAddr addr, size_t num) : addr(addr), numPages(num)
	{
	}

	VMRegion::VMRegion(const VMRegion& other)
		: addr(other.addr), numPages(other.numPages), backingPhysPages(other.backingPhysPages)
	{
	}

	VMRegion::VMRegion(VMRegion&& other)
	{
		this->addr = other.addr;            other.addr.clear();
		this->numPages = other.numPages;    other.numPages = 0;

		this->backingPhysPages = krt::move(other.backingPhysPages);
	}


	VMRegion& VMRegion::operator = (const VMRegion& other)
	{
		if(this != &other)  return *this = VMRegion(other);
		else                return *this;
	}

	VMRegion& VMRegion::operator = (VMRegion&& other)
	{
		if(this != &other)
		{
			this->addr = other.addr;            other.addr.clear();
			this->numPages = other.numPages;    other.numPages = 0;
			this->backingPhysPages = krt::move(other.backingPhysPages);
		}

		return *this;
	}

	VirtAddr VMRegion::end() const
	{
		return this->addr + ofsPages(this->numPages);
	}

	void VMRegion::grow_down(size_t count)
	{
		this->addr -= ofsPages(count);
		this->numPages += count;
	}

	void VMRegion::grow_up(size_t count)
	{
		this->numPages += count;
	}

	void VMRegion::shrink_down(size_t count)
	{
		assert(count <= this->numPages);

		for(size_t i = 0; i < count; i++)
			this->backingPhysPages.remove(this->end() + ofsPages(i));

		this->numPages -= count;
	}

	void VMRegion::shrink_up(size_t count)
	{
		assert(count <= this->numPages);

		for(size_t i = 0; i < count; i++)
			this->backingPhysPages.remove(this->addr + ofsPages(i));

		this->addr += ofsPages(count);
		this->numPages -= count;
	}





}
}

















