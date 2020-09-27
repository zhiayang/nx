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


	void AddressSpace::LockedAddrSpace::init(PhysAddr cr3)
	{
		if(cr3.nonZero())   this->addrspace.theCR3 = cr3;
		else                this->addrspace.theCR3 = pmm::allocate(1);
	}

	void AddressSpace::LockedAddrSpace::destroy()
	{
		size_t num = 1;
		pmm::deallocate(this->addrspace.cr3(), 1);

		for(auto x : this->addrspace.allocatedPhysPages)
			num++, pmm::deallocate(x, 1);

		// destroy each region.
		for(const auto& r : this->addrspace.regions)
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

		log("adrspc", "freed {} physical pages", num);
	}

	PhysAddr AddressSpace::cr3()
	{
		return this->theCR3;
	}

	extmm::State<>& AddressSpace::getVMMState(AddressSpaceType addrspc)
	{
		return this->vmmStates[(size_t) addrspc];
	}

	void AddressSpace::LockedAddrSpace::addTrackedPhysPage(PhysAddr addr)
	{
		this->addrspace.allocatedPhysPages.append(addr);
	}

	void AddressSpace::LockedAddrSpace::addTrackedPhysPages(PhysAddr base, size_t count)
	{
		for(size_t i = 0; i < count; i++)
			this->addrspace.allocatedPhysPages.append(base + ofsPages(i));
	}

	void AddressSpace::LockedAddrSpace::removeTrackedPhysPage(PhysAddr addr)
	{
		this->addrspace.allocatedPhysPages.remove_all(addr);
	}

	void AddressSpace::LockedAddrSpace::removeTrackedPhysPages(PhysAddr base, size_t count)
	{
		for(size_t i = 0; i < count; i++)
			this->addrspace.allocatedPhysPages.remove_all(base + ofsPages(i));
	}



	void AddressSpace::LockedAddrSpace::addRegion(VirtAddr addr, size_t num)
	{
		assert(heap::initialised());
		assert(addr.isPageAligned());

		for(auto& vmr : this->addrspace.regions)
		{
			if(end(addr, num) == vmr.addr)
			{
				vmr.grow_down(num);
				return;
			}
			else if(vmr.end() == addr)
			{
				vmr.grow_up(num);
				return;
			}
			else if(vmr.addr == addr)
			{
				// hmm...
				abort("vmregion: potential double free! addr: {p}, size: {}", addr, num);
			}
		}

		this->addrspace.regions.emplace(addr, num);
	}

	void AddressSpace::LockedAddrSpace::addRegion(VirtAddr addr, PhysAddr physStart, size_t num)
	{
		assert(heap::initialised());
		assert(addr.isPageAligned() && physStart.isPageAligned());

		auto update_phys = [](VMRegion& vmr, VirtAddr virt, PhysAddr phys, size_t num) {
			for(size_t i = 0; i < num; i++)
				vmr.backingPhysPages[virt + ofsPages(i)] = phys + ofsPages(i);
		};

		for(auto& vmr : this->addrspace.regions)
		{
			if(end(addr, num) == vmr.addr)
			{
				vmr.grow_down(num);
				update_phys(vmr, vmr.addr, physStart, num);

				return;
			}
			else if(vmr.end() == addr)
			{
				auto oldSz = vmr.numPages;

				vmr.grow_up(num);
				update_phys(vmr, vmr.addr + ofsPages(oldSz), physStart, num);

				return;
			}
			else if(vmr.addr == addr)
			{
				// hmm...
				abort("vmregion: potential double add! addr: {p}, phys: {p}, size: {}/{}", addr, physStart, num, vmr.numPages);
			}
		}


		auto vmr = VMRegion(addr, num);
		update_phys(vmr, vmr.addr, physStart, num);

		this->addrspace.regions.append(krt::move(vmr));
	}

	void AddressSpace::LockedAddrSpace::addSharedRegion(SharedVMRegion&& region)
	{
		if(this->addrspace.sharedRegions.find(region) != this->addrspace.sharedRegions.end())
			abort("vmregion: adding duplicate region!");

		this->addrspace.sharedRegions.append(krt::move(region));
	}

	void AddressSpace::LockedAddrSpace::removeSharedRegion(SharedVMRegion&& region)
	{
		this->addrspace.sharedRegions.remove_all(region);
	}

	void AddressSpace::LockedAddrSpace::freeRegion(VirtAddr addr, size_t num, bool freePhys)
	{
		assert(heap::initialised());
		assert(addr.isPageAligned());

		for(auto it = this->addrspace.regions.begin(); it != this->addrspace.regions.end(); ++it)
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
						this->addrspace.regions.erase(it);

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
						this->addrspace.regions.erase(it);

					return;
				}
				else
				{
					// bollocks, it's somewhere in the middle.
					size_t numFrontPages = (addr - vmr.addr) / PAGE_SIZE;
					size_t numBackPages = (vmr.end() - end(addr, num)) / PAGE_SIZE;

					assert(numFrontPages > 0 && numBackPages > 0);

					auto back_vmr = vmr.clone();

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
						this->addrspace.regions.erase(it);

					// shrink_up the back vmr:
					back_vmr.shrink_up(numFrontPages + num);
					if(back_vmr.numPages > 0)
						this->addrspace.regions.append(krt::move(back_vmr));

					return;
				}
			}
		}

		abort("vmregion: did not find matching extent for ({p}, {})", addr, num);
	}

	PhysAddr AddressSpace::LockedAddrSpace::addPhysicalMapping(VirtAddr virt, PhysAddr phys)
	{
		for(auto it = this->addrspace.regions.begin(); it != this->addrspace.regions.end(); ++it)
		{
			auto& vmr = *it;

			if(vmr.addr <= virt && virt < vmr.end())
			{
				PhysAddr old;
				if(auto it = vmr.backingPhysPages.find(virt); it != vmr.backingPhysPages.end())
					old = it->value;

				vmr.backingPhysPages[virt] = phys;
				return old;
			}
		}

		for(auto vmr = this->addrspace.sharedRegions.begin(); vmr != this->addrspace.sharedRegions.end(); ++vmr)
		{
			if(vmr->addr <= virt && virt < vmr->end())
			{
				// the shared regions deal in indices, because the virtual address can shift around.
				// so the key for the hashmap is the "index" of the virt page, from the beginning of the region.
				auto pageIdx = (virt - vmr->addr) / PAGE_SIZE;

				// since the backing physical region is shared, it has its own lock.
				return LockedSection(&vmr->backingPhysPages->mtx, [&]() -> auto {
					PhysAddr old;
					if(auto it = vmr->backingPhysPages->physPages.find(pageIdx); it != vmr->backingPhysPages->physPages.end())
						old = it->value;

					vmr->backingPhysPages->physPages[pageIdx] = phys;
					return old;
				});
			}
		}

		abort("vmregion: did not find matching extent for ({p})", virt);
		return PhysAddr::zero();
	}

	krt::pair<optional<VirtAddr>, optional<PhysAddr>> AddressSpace::LockedAddrSpace::lookupVirtualMapping(VirtAddr addr)
	{
		auto contains = [](const auto& r, VirtAddr v) -> bool {
			return r.addr <= v && v < r.end();
		};

		// it is more likely that this will involve the shared mappings, so search those first.
		for(auto& svmr : this->addrspace.sharedRegions)
		{
			if(contains(svmr, addr))
				return { opt::some(addr), svmr.lookup(addr) };
		}

		// oof
		for(auto& vmr : this->addrspace.regions)
		{
			if(contains(vmr, addr))
				return { opt::some(addr), vmr.lookup(addr) };
		}

		return { opt::none, opt::none };
	}





	VMRegion::VMRegion(VirtAddr addr, size_t num) : addr(addr), numPages(num)
	{
	}

	VMRegion::VMRegion(VMRegion&& other)
	{
		this->addr = other.addr;            other.addr.clear();
		this->numPages = other.numPages;    other.numPages = 0;

		this->backingPhysPages = krt::move(other.backingPhysPages);
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

	VMRegion VMRegion::clone() const
	{
		auto ret = VMRegion(this->addr, this->numPages);
		ret.backingPhysPages = this->backingPhysPages;

		return ret;
	}

	VirtAddr VMRegion::end() const
	{
		return this->addr + ofsPages(this->numPages);
	}

	optional<PhysAddr> VMRegion::lookup(VirtAddr virt) const
	{
		auto it = this->backingPhysPages.find(virt);
		return (it == this->backingPhysPages.end()
			? opt::none
			: opt::some(it->value)
		);
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



	SharedVMRegion::SharedVMRegion(VirtAddr addr, size_t num, SharedPhysRegion* store) : addr(addr), numPages(num), backingPhysPages(store)
	{
	}

	SharedVMRegion::SharedVMRegion(SharedVMRegion&& other)
	{
		this->addr = other.addr;
		other.addr.clear();

		this->numPages = other.numPages;
		other.numPages = 0;

		this->backingPhysPages = other.backingPhysPages;
		other.backingPhysPages = nullptr;
	}

	SharedVMRegion& SharedVMRegion::operator = (SharedVMRegion&& other)
	{
		if(this != &other)
		{
			this->addr = other.addr;
			other.addr.clear();

			this->numPages = other.numPages;
			other.numPages = 0;

			this->backingPhysPages = other.backingPhysPages;
			other.backingPhysPages = nullptr;
		}

		return *this;
	}

	VirtAddr SharedVMRegion::end() const
	{
		return this->addr + ofsPages(this->numPages);
	}

	SharedVMRegion SharedVMRegion::clone() const
	{
		return SharedVMRegion(this->addr, this->numPages, this->backingPhysPages);
	}

	optional<PhysAddr> SharedVMRegion::lookup(VirtAddr virt) const
	{
		auto it = this->backingPhysPages->physPages.find((virt - this->addr) / PAGE_SIZE);
		return (it == this->backingPhysPages->physPages.end()
			? opt::none
			: opt::some(it->value)
		);
	}

	void SharedVMRegion::SharedPhysRegion::destroy()
	{
		// we just free all the physical pages.
		for(auto p : this->physPages)
			pmm::deallocate(p.second, 1);
	}
}
}

















