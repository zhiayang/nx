// operators.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "mm.h"

[[nodiscard]] void* operator new    (size_t count)              { return (void*) nx::heap::allocate(count, 1); }
[[nodiscard]] void* operator new[]  (size_t count)              { return (void*) nx::heap::allocate(count, 1); }
[[nodiscard]] void* operator new    (size_t count, size_t al)   { return (void*) nx::heap::allocate(count, al); }
[[nodiscard]] void* operator new[]  (size_t count, size_t al)   { return (void*) nx::heap::allocate(count, al); }
[[nodiscard]] void* operator new    (size_t count, void* ptr)   { return ptr; }
[[nodiscard]] void* operator new[]  (size_t count, void* ptr)   { return ptr; }

void operator delete    (void* ptr)                             { nx::heap::deallocate((addr_t) ptr); }
void operator delete[]  (void* ptr)                             { nx::heap::deallocate((addr_t) ptr); }
void operator delete    (void* ptr, size_t al)                  { }
void operator delete[]  (void* ptr, size_t al)                  { }