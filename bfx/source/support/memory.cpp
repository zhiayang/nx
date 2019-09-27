// memory.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "bfx.h"

namespace bfx
{
	void* allocator::allocate(size_t sz, size_t align)  { return 0; }
	void allocator::deallocate(void* ptr)               { }
}






#ifdef FUCKIN_WSL_FIX_YOUR_SHIT
[[nodiscard]] void* operator new    (unsigned long long count, void* ptr)   { return ptr; }
[[nodiscard]] void* operator new[]  (unsigned long long count, void* ptr)   { return ptr; }
#else
[[nodiscard]] void* operator new    (size_t count, void* ptr)   { return ptr; }
[[nodiscard]] void* operator new[]  (size_t count, void* ptr)   { return ptr; }
#endif
