// arraylike.h
// Copyright (c) 2018, zhiayang
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
	template <typename Container, typename ElmTy, typename allocator, typename aborter>
	struct arraylike_impl
	{
		static void reserve(Container* self, size_t atleast)
		{
			if constexpr (krt::is_same<ElmTy, char>())
				atleast += 1;


			if(self->cap >= atleast) return;
			size_t newsz = __max(16, __max(atleast, (self->cap * 3) / 2));

			// if T is like char16_t or some shit, we need to account for it
			auto newptr = (ElmTy*) allocator::allocate(newsz * sizeof(ElmTy), alignof(ElmTy));
			if(!newptr) aborter::abort("reserve(): alloc() returned null!");

			copy_elements(self, newptr, self->ptr, self->cnt);

			allocator::deallocate(self->ptr);

			self->ptr = newptr;
			self->cap = newsz;
		}

		static ElmTy& front(Container* self)
		{
			if(self->cnt == 0) aborter::abort("front(): empty array");
			return self->ptr[0];
		}

		static ElmTy& back(Container* self)
		{
			if(self->cnt == 0) aborter::abort("back(): empty array");
			return self->ptr[self->cnt - 1];
		}

		static const ElmTy& front_const(const Container* self)
		{
			if(self->cnt == 0) aborter::abort("front(): empty array");
			return self->ptr[0];
		}

		static const ElmTy& back_const(const Container* self)
		{
			if(self->cnt == 0) aborter::abort("back(): empty array");
			return self->ptr[self->cnt - 1];
		}

		static void clear(Container* self)
		{
			for(size_t i = 0; i < self->cnt; i++)
				self->ptr[i].~ElmTy();

			self->cnt = 0;
		}

		static Container subarray(const Container* self, size_t idx, size_t len)
		{
			if(len == -1) len = self->cnt - idx;

			if(idx >= self->cnt || idx + len > self->cnt)
				aborter::abort("subarray(): out of range wanted (%zu, %zu), length is only %d", idx, len, self->cnt);

			// ok, we should be good.
			return Container(self->ptr + idx, len);
		}

		static Container& append_element(Container* self, const ElmTy& c)
		{
			self->reserve(self->cnt + 1);

			new (&self->ptr[self->cnt]) ElmTy(c);
			self->cnt++;

			set_last_if_char(self);
			return *self;
		}

		static Container& append(Container* self, const Container& s)
		{
			self->reserve(self->cnt + s.cnt);

			copy_elements(self, self->ptr + self->cnt, s.ptr, s.cnt);

			self->cnt += s.cnt;

			set_last_if_char(self);
			return *self;
		}

		static void erase_at(Container* self, size_t idx, size_t num)
		{
			if(idx >= self->cnt) aborter::abort("erase_at(): index %zu out of range (size: %zu)", idx, self->cnt);
			if(idx + num > self->cnt) aborter::abort("erase_at(): not enough items to erase (wanted %zu from idx %zu, only have %zu)",
				num, idx, self->cnt - idx);

			if(idx + num == self->cnt)
			{
				// very easy, just remove those from the back.
				for(size_t i = idx; i < idx + num; i++)
					self->ptr[i].~ElmTy();

				self->cnt -= num;
			}
			else
			{
				// oops, not so easy.
				// we need to shift things around. the standard array guarantees order,
				// we might make an unord_array that can do self more efficiently (by moving items
				// from the back to fill in the gap.)

				// first destroy the stuff we wanted to destroy
				for(size_t i = idx; i < idx + num; i++)
					self->ptr[i].~ElmTy();

				// then start moving stuff
				for(size_t i = idx; i < self->cnt; i++)
					self->ptr[i] = krt::move(self->ptr[i + num]);

				// update the count
				self->cnt -= num;
			}
		}


		static bool op_cmpeq(const Container* self, const Container& other)
		{
			if(self->cnt != other.cnt) return false;
			for(size_t i = 0; i < self->cnt; i++)
			{
				if(self->ptr[i] != other.ptr[i])
					return false;
			}

			return true;
		}

		static bool op_cmplt(const Container* self, const Container& other)
		{
			if(self->cnt != other.cnt) return false;

			// sadly we do not have the min macro.
			for(size_t i = 0; i < (other.cnt < self->cnt ? other.cnt : self->cnt); i++)
			{
				if(self->ptr[i] < other.ptr[i])
					return true;

				else if(self->ptr[i] > other.ptr[i])
					return false;
			}

			// well they were equal -- we return the shorter array.
			return self->cnt < other.cnt;
		}

		static bool op_cmpgt(const Container* self, const Container& other) { return (other < *self); }
		static bool op_cmpneq(const Container* self, const Container& other) { return !(*self == other); }
		static bool op_cmpgeq(const Container* self, const Container& other) { return !(*self < other); }
		static bool op_cmpleq(const Container* self, const Container& other) { return !(other < *self); }

		static ElmTy& op_subscript(Container* self, size_t idx)
		{
			if(idx >= self->cnt) aborter::abort("operator[]: index %zu out of range (size: %zu)", idx, self->cnt);
			return self->ptr[idx];
		}

		static const ElmTy& op_subscript_const(const Container* self, size_t idx)
		{
			if(idx >= self->cnt) aborter::abort("operator[]: index %zu out of range (size: %zu)", idx, self->cnt);
			return self->ptr[idx];
		}


		private:
		static void set_last_if_char(Container* self)
		{
			if constexpr (krt::is_same<ElmTy, char>())
				self->ptr[self->cnt] = 0;
		}

		static void copy_elements(Container* self, ElmTy* dest, ElmTy* src, size_t num)
		{
			if(krt::is_trivially_copyable<ElmTy>::value)
			{
				memmove(dest, src, num);
			}
			else
			{
				for(size_t i = 0; i < num; i++)
					dest[i] = src[i];
			}
		}
	};
}













