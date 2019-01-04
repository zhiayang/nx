// operators.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "defs.h"
#include "mm.h"


[[nodiscard]] void* operator new    (size_t count)              { return (void*) nx::heap::allocate(count, 1); }
[[nodiscard]] void* operator new[]  (size_t count)              { return (void*) nx::heap::allocate(count, 1); }
[[nodiscard]] void* operator new    (size_t count, size_t al)   { return (void*) nx::heap::allocate(count, al); }
[[nodiscard]] void* operator new[]  (size_t count, size_t al)   { return (void*) nx::heap::allocate(count, al); }
[[nodiscard]] void* operator new    (size_t count, void* ptr)   { return ptr; }
[[nodiscard]] void* operator new[]  (size_t count, void* ptr)   { return ptr; }

void operator delete    (void* ptr) noexcept                    { nx::heap::deallocate((addr_t) ptr); }
void operator delete[]  (void* ptr) noexcept                    { nx::heap::deallocate((addr_t) ptr); }
void operator delete    (void* ptr, size_t al)                  { }
void operator delete[]  (void* ptr, size_t al)                  { }





extern "C"
{
	void __cxa_pure_virtual()
	{
		nx::abort("tried to call pure virtual function!");
	}


	struct atexit_func_entry_t
	{
		/*
		* Each member is at least 4 bytes large. Such that each entry is 12bytes.
		* 128 * 12 = 1.5KB exact.
		**/
		void (*destructor_func)(void* );
		void* obj_ptr;
		void* dso_handle;
	};

	volatile void* __dso_handle;
	atexit_func_entry_t __atexit_funcs[128];
	unsigned __atexit_func_count = 0;


	int __cxa_atexit(void (*f)(void*), void* objptr, void* dso)
	{
		if(__atexit_func_count >= 128)
			return -1;

		__atexit_funcs[__atexit_func_count].destructor_func = f;
		__atexit_funcs[__atexit_func_count].obj_ptr = objptr;
		__atexit_funcs[__atexit_func_count].dso_handle = dso;
		__atexit_func_count++;

		return 0;
	}

	void __cxa_finalize(void* f)
	{
		auto i = __atexit_func_count;
		if(!f)
		{
			while(i--)
			{
				if(__atexit_funcs[i].destructor_func)
					(*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
			}

			return;
		}

		while(i--)
		{
			if((void*) __atexit_funcs[i].destructor_func == f)
			{
				(*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
					__atexit_funcs[i].destructor_func = 0;
			}
		}
	}
}












