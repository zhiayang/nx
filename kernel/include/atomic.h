// atomic.h
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <stddef.h>

namespace nx
{
	namespace atomic
	{
		bool cas16(void* ptr, void* expected_value, void* new_value);

		void cas_spinlock(uint64_t* ptr, uint64_t expected, uint64_t new_value);
		bool cas_trylock(uint64_t* ptr, uint64_t expected, uint64_t new_value);

		void cas_spinlock_noirq(uint64_t* ptr, uint64_t expected, uint64_t new_value);
		bool cas_trylock_noirq(uint64_t* ptr, uint64_t expected, uint64_t new_value);


		bool cas_fetch(int8_t* ptr, int8_t* expected_value, int8_t new_value);
		bool cas_fetch(int16_t* ptr, int16_t* expected_value, int16_t new_value);
		bool cas_fetch(int32_t* ptr, int32_t* expected_value, int32_t new_value);
		bool cas_fetch(int64_t* ptr, int64_t* expected_value, int64_t new_value);
		bool cas_fetch(uint8_t* ptr, uint8_t* expected_value, uint8_t new_value);
		bool cas_fetch(uint16_t* ptr, uint16_t* expected_value, uint16_t new_value);
		bool cas_fetch(uint32_t* ptr, uint32_t* expected_value, uint32_t new_value);
		bool cas_fetch(uint64_t* ptr, uint64_t* expected_value, uint64_t new_value);

		bool cas(int8_t* ptr, int8_t expected_value, int8_t new_value);
		bool cas(int16_t* ptr, int16_t expected_value, int16_t new_value);
		bool cas(int32_t* ptr, int32_t expected_value, int32_t new_value);
		bool cas(int64_t* ptr, int64_t expected_value, int64_t new_value);
		bool cas(uint8_t* ptr, uint8_t expected_value, uint8_t new_value);
		bool cas(uint16_t* ptr, uint16_t expected_value, uint16_t new_value);
		bool cas(uint32_t* ptr, uint32_t expected_value, uint32_t new_value);
		bool cas(uint64_t* ptr, uint64_t expected_value, uint64_t new_value);

		void incr(int8_t* ptr);
		void incr(int16_t* ptr);
		void incr(int32_t* ptr);
		void incr(int64_t* ptr);
		void incr(uint8_t* ptr);
		void incr(uint16_t* ptr);
		void incr(uint32_t* ptr);
		void incr(uint64_t* ptr);

		void decr(int8_t* ptr);
		void decr(int16_t* ptr);
		void decr(int32_t* ptr);
		void decr(int64_t* ptr);
		void decr(uint8_t* ptr);
		void decr(uint16_t* ptr);
		void decr(uint32_t* ptr);
		void decr(uint64_t* ptr);

		int8_t load(int8_t* ptr);
		int16_t load(int16_t* ptr);
		int32_t load(int32_t* ptr);
		int64_t load(int64_t* ptr);
		uint8_t load(uint8_t* ptr);
		uint16_t load(uint16_t* ptr);
		uint32_t load(uint32_t* ptr);
		uint64_t load(uint64_t* ptr);

		// because of type-system problems, we cannot have the first parameter as void** and still let the thing work
		// so we just use void*, and treat that void* as a void**.
		void store(void* ptr, void* value);

		void store(int8_t* ptr, int8_t value);
		void store(int16_t* ptr, int16_t value);
		void store(int32_t* ptr, int32_t value);
		void store(int64_t* ptr, int64_t value);
		void store(uint8_t* ptr, uint8_t value);
		void store(uint16_t* ptr, uint16_t value);
		void store(uint32_t* ptr, uint32_t value);
		void store(uint64_t* ptr, uint64_t value);
	}
}
