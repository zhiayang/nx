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
	template <typename Container, typename ElmTy, typename allocator, typename aborter, size_t sso_buf_size = 0>
	struct arraylike_impl
	{
		static bool reserve(Container* self, size_t atleast)
		{
			if constexpr (std::is_same_v<ElmTy, char>)
				atleast += 1;

			if(self->cap >= atleast) return false;
			size_t newsz = __max((size_t) 16, __max(atleast, (self->cap * 3) / 2));


			auto newptr = allocator::template allocate<ElmTy>(newsz);
			if(!newptr) aborter::abort("reserve(): alloc() returned null!");

			copy_elements(self, newptr, self->ptr, self->cnt);

			self->ptr = newptr;
			self->cap = newsz;

			// lets the actual impl know that we allocated a new buffer.
			// up to them to free the memory, because i don't want to care too much about the small-string/vector-opt
			// details here.
			return true;
		}

		// this does NOT shrink the memory block! it only changes the size.
		static bool resize(Container* self, size_t size)
		{
			if(size < self->cnt)
			{
				for(size_t i = size; i < self->cnt; i++)
					(self->ptr + i)->~ElmTy();

				self->cnt = size;
				return false;
			}
			else if(size > self->cnt)
			{
				// this does most of the work. we just need to insert new elements.
				bool did = reserve(self, size);

				// insert default constructed elements
				for(size_t i = self->cnt; i < size; i++)
					new (self->ptr + i) ElmTy();

				// change the size
				self->cnt = size;

				return did;
			}

			return false;
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
			if(len == (size_t) -1) len = self->cnt - idx;

			if(idx > self->cnt || idx + len > self->cnt)
				aborter::abort("subarray(): out of range; wanted (%zu, %zu), length is only %d", idx, len, self->cnt);

			// ok, we should be good.
			return Container(self->ptr + idx, len);
		}

		template <typename... Ts>
		static Container& emplace_element(Container* self, Ts&&... args)
		{
			self->reserve(self->cnt + 1);

			new (&self->ptr[self->cnt]) ElmTy(args...);
			self->cnt++;

			set_last_if_char(self);
			return *self;
		}

		static Container& append_element(Container* self, const ElmTy& c)
		{
			self->reserve(self->cnt + 1);

			new (&self->ptr[self->cnt]) ElmTy(c);
			self->cnt++;

			set_last_if_char(self);
			return *self;
		}

		static Container& append_element(Container* self, ElmTy&& c)
		{
			self->reserve(self->cnt + 1);

			new (&self->ptr[self->cnt]) ElmTy(move(c));
			self->cnt++;

			set_last_if_char(self);
			return *self;
		}


		static Container& append(Container* self, const ElmTy* xs, size_t count)
		{
			self->reserve(self->cnt + count);

			while(count--)
				self->ptr[self->cnt++] = *xs++;

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

		static Container& append(Container* self, Container&& s)
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


		static bool op_cmpeq(const Container* self, const ElmTy* other, size_t count)
		{
			if(self->cnt != count) return false;
			for(size_t i = 0; i < self->cnt; i++)
			{
				if(self->ptr[i] != other[i])
					return false;
			}

			return true;
		}

		static bool op_cmplt(const Container* self, const ElmTy* other, size_t count)
		{
			if(self->cnt != count) return false;

			// sadly we do not have the min macro.
			for(size_t i = 0; i < (count < self->cnt ? count : self->cnt); i++)
			{
				if(self->ptr[i] < other[i])
					return true;

				else if(self->ptr[i] > other[i])
					return false;
			}

			// well they were equal -- we return the shorter array.
			return self->cnt < count;
		}


		static bool op_cmpeq(const Container* self, const Container& other)
		{
			return op_cmpeq(self, other.ptr, other.cnt);
		}

		static bool op_cmplt(const Container* self, const Container& other)
		{
			return op_cmplt(self, other.ptr, other.cnt);
		}

		static bool op_cmpgt(const Container* self, const Container& other) { return (other < *self); }
		static bool op_cmpneq(const Container* self, const Container& other) { return !(*self == other); }
		static bool op_cmpgeq(const Container* self, const Container& other) { return !(*self < other); }
		static bool op_cmpleq(const Container* self, const Container& other) { return !(other < *self); }

		static bool op_cmpgt(const Container* self, const ElmTy* other) { return (other < *self); }
		static bool op_cmpneq(const Container* self, const ElmTy* other) { return !(*self == other); }
		static bool op_cmpgeq(const Container* self, const ElmTy* other) { return !(*self < other); }
		static bool op_cmpleq(const Container* self, const ElmTy* other) { return !(other < *self); }

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


		static void copy_elements(Container* self, ElmTy* dest, ElmTy* src, size_t num)
		{
			{
				using namespace std;
				static_assert(is_trivially_copyable_v<ElmTy> || is_copy_constructible_v<ElmTy> || is_move_constructible_v<ElmTy>,
					"element type cannot be constructed");
			}

			if constexpr (std::is_trivially_copyable_v<ElmTy>)
			{
				memmove(dest, src, num * sizeof(ElmTy));
			}
			else if constexpr (std::is_copy_constructible_v<ElmTy>)
			{
				for(size_t i = 0; i < num; i++)
					new (&dest[i]) ElmTy(src[i]);
			}
			else if constexpr (std::is_move_constructible_v<ElmTy>)
			{
				for(size_t i = 0; i < num; i++)
					new (&dest[i]) ElmTy(move(src[i]));
			}
		}

	private:
		static void set_last_if_char(Container* self)
		{
			if constexpr (std::is_same_v<ElmTy, char>)
				self->ptr[self->cnt] = 0;
		}

	};
}













