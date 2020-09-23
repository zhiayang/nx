// hashset.h
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "impl/bucket_hashmap.h"

namespace krt
{
	template <typename KeyTy, typename allocator, typename aborter, typename Hasher = krt::hash<KeyTy>>
	using bucket_hashset = bucket_hashmap<KeyTy, void, allocator, aborter, Hasher>;
}
