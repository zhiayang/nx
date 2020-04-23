// iterutils.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"
#include "string.h"

#include "krt/meta.h"
#include "krt/macros.h"
#include "krt/iterators.h"

namespace krt
{
	// is this what the cool kids call CRTP??
	template <typename T, typename E>
	struct iter_find_fns
	{
		// using E     = typename iter_traits<T, Alloc, Abort>::elem_type;
		// using It    = typename iter_traits<T, Alloc, Abort>::iterator;
		// using CIt   = typename iter_traits<T, Alloc, Abort>::const_iterator;

		auto find(const E& x)
		{
			auto self = static_cast<T*>(this);
			for(auto it = self->begin(); it != self->end(); ++it)
				if(*it == x) { return it; }

			return self->end();
		}

		auto find(const E& x) const
		{
			auto self = static_cast<T*>(this);
			for(auto it = self->begin(); it != self->end(); ++it)
				if(*it == x) { return it; }

			return self->end();
		}

		template <typename Predicate>
		auto find_if(Predicate p)
		{
			auto self = static_cast<T*>(this);
			for(auto it = self->begin(); it != self->end(); ++it)
				if(p(*it)) { return it; }

			return self->end();
		}

		template <typename Predicate>
		auto find_if(Predicate p) const
		{
			auto self = static_cast<T*>(this);
			for(auto it = self->begin(); it != self->end(); ++it)
				if(p(*it)) { return it; }

			return self->end();
		}
	};


	template <typename T, typename E>
	struct iter_remove_fns
	{
		T& remove_first(const E& x)
		{
			auto self = static_cast<T*>(this);
			for(auto it = self->begin(); it != self->end(); ++it)
				if(*it == x) { self->erase(it); break; }

			return *self;
		}

		T& remove_all(const E& x)
		{
			auto self = static_cast<T*>(this);

			auto it = self->begin();
			while(it != self->end())
			{
				if(*it == x)    it = self->erase(it);
				else            ++it;
			}

			return *self;
		}

		T& remove_first(E&& x)
		{
			auto self = static_cast<T*>(this);
			for(auto it = self->begin(); it != self->end(); ++it)
				if(*it == x) { self->erase(it); break; }

			return *self;
		}

		T& remove_all(E&& x)
		{
			auto self = static_cast<T*>(this);

			auto it = self->begin();
			while(it != self->end())
			{
				if(*it == x)    it = self->erase(it);
				else            ++it;
			}

			return *self;
		}


		template <typename Predicate>
		T& remove_first_if(Predicate p)
		{
			auto self = static_cast<T*>(this);

			for(auto it = self->begin(); it != self->end(); ++it)
				if(p(*it)) { self->erase(it); break; }

			return *self;
		}

		template <typename Predicate>
		T& remove_all_if(Predicate p)
		{
			auto self = static_cast<T*>(this);

			auto it = self->begin();
			while(it != self->end())
			{
				if(p(*it))  it = self->erase(it);
				else        ++it;
			}

			return *self;
		}
	};
}






