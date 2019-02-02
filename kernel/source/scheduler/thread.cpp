// thread.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace scheduler
{
	static constexpr size_t KernelStackSize = 0x4000;

	Thread* createThread(Process* p, Fn0Args_t fn)
	{
		return createThread(p, (Fn6Args_t) fn, 0, 0, 0, 0, 0, 0);
	}
	Thread* createThread(Process* p, Fn1Arg_t fn, void* a)
	{
		return createThread(p, (Fn6Args_t) fn, a, 0, 0, 0, 0, 0);
	}
	Thread* createThread(Process* p, Fn2Args_t fn, void* a, void* b)
	{
		return createThread(p, (Fn6Args_t) fn, a, b, 0, 0, 0, 0);
	}
	Thread* createThread(Process* p, Fn3Args_t fn, void* a, void* b, void* c)
	{
		return createThread(p, (Fn6Args_t) fn, a, b, c, 0, 0, 0);
	}
	Thread* createThread(Process* p, Fn4Args_t fn, void* a, void* b, void* c, void* d)
	{
		return createThread(p, (Fn6Args_t) fn, a, b, c, d, 0, 0);
	}
	Thread* createThread(Process* p, Fn5Args_t fn, void* a, void* b, void* c, void* d, void* e)
	{
		return createThread(p, (Fn6Args_t) fn, a, b, c, d, e, 0);
	}



	static constexpr uint64_t RING0_CODE_SEGMENT        = 0x08;
	static constexpr uint64_t RING0_STACK_SEGMENT       = 0x10;

	static constexpr uint64_t RING3_CODE_SEGMENT        = 0x23;     // 0x20 | 0x3
	static constexpr uint64_t RING3_STACK_SEGMENT       = 0x1B;     // 0x18 | 0x3

	//* note: we don't bother rounding this up, we just divide by PAGE_SIZE
	//* so don't be stupid; set these to a multiple of 0x1000, thanks.
	static constexpr size_t KERNEL_STACK_SIZE           = 0x2000;
	static constexpr size_t USER_STACK_SIZE             = 0x4000;

	Thread* createThread(Process* proc, Fn6Args_t fn, void* a, void* b, void* c, void* d, void* e, void* f)
	{
		bool isKernProc = (proc == scheduler::getKernelProcess());

		// setup the kernel stack

		assert(proc);
		proc->threads.append(Thread());

		auto thr = &proc->threads.back();
		thr->parent = proc;


		// allocate the user and kernel stacks, but in the address space of the process.
		// we don't really care what the address is, as long as we got the space.
		// we do the phys and virt allocation separately because we want the physical address as well.

		//? note: we allocate everything in the 'user' AddressSpace in the current process, because the kernel might decide to change
		//? mappings while in a syscall or something -- we might want to create threads or something while using another
		//? CR3, so we need to take note of that.

		// first, just make some space for the user stack.
		{
			auto n = USER_STACK_SIZE / PAGE_SIZE;

			auto phys = pmm::allocate(n);
			auto virt = vmm::allocateAddrSpace(n, vmm::AddressSpace::User, proc);

			// map it in the target address space
			vmm::mapAddress(virt, phys, n, vmm::PAGE_PRESENT | (isKernProc ? 0 : (vmm::PAGE_WRITE | vmm::PAGE_USER)));

			// we don't need to map it in our address space, because we don't need to write anything there.
			// (yet!) -- eventually we want to write a return address that will kill the thread.
			thr->userStack = virt;

			println("usr stack done");
		}


		// then, the kernel stack:
		{
			auto n = KERNEL_STACK_SIZE / PAGE_SIZE;

			auto phys = pmm::allocate(n);
			auto virt = vmm::allocateAddrSpace(n, vmm::AddressSpace::User, proc);

			// map it in the target address space
			vmm::mapAddress(virt, phys, n, vmm::PAGE_PRESENT | (isKernProc ? 0 : (vmm::PAGE_WRITE | vmm::PAGE_USER)));

			println("kern stack done");

			// ok, now allocate some space here -- as scratch so we can modify the pages.
			auto scratch = vmm::allocateAddrSpace(n, vmm::AddressSpace::User);

			vmm::mapAddress(scratch, phys, n, vmm::PAGE_PRESENT);

			println("kern stack done");

			auto kstk = (uint64_t*) scratch;

			*--kstk     = (isKernProc ? RING0_STACK_SEGMENT : RING3_STACK_SEGMENT);     // stack segment
			*--kstk     = thr->userStack;                                               // stack pointer
			*--kstk     = 0x202;                                                        // flags
			*--kstk     = (isKernProc ? RING0_CODE_SEGMENT : RING3_CODE_SEGMENT);       // code segment
			*--kstk     = (addr_t) fn;                                                  // return addr


			// now for the registers.
			*--kstk     = 0;            // r15
			*--kstk     = 0;            // r14
			*--kstk     = 0;            // r13
			*--kstk     = 0;            // r12
			*--kstk     = 0;            // r11
			*--kstk     = 0;            // r10
			*--kstk     = (uint64_t) f; // r9
			*--kstk     = (uint64_t) e; // r8
			*--kstk     = (uint64_t) c; // rdx
			*--kstk     = (uint64_t) d; // rcx
			*--kstk     = 0;            // rbx
			*--kstk     = 0;            // rax
			*--kstk     = 0;            // rbp
			*--kstk     = (uint64_t) b; // rsi
			*--kstk     = (uint64_t) a; // rdi

			// should be 160
			assert((addr_t) kstk + 160 == scratch);

			// ok. set the stack.
			thr->kernelStack = virt - 160;
		}

		return thr;
	}
}
}



















