// priority.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace scheduler
{
	// note: because lower priority threads that get boosted probably
	// have a good reason, we make the boost do more.

	constexpr uint64_t LOW_BASE_PRIORITY        = 50000;
	constexpr uint64_t LOW_STARVATION_DIV       = 9000;
	constexpr uint64_t LOW_BOOST_MULT           = 18000;

	constexpr uint64_t NORMAL_BASE_PRIORITY     = 100000;
	constexpr uint64_t NORMAL_STARVATION_DIV    = 8000;
	constexpr uint64_t NORMAL_BOOST_MULT        = 15000;

	constexpr uint64_t HIGH_BASE_PRIORITY       = 150000;
	constexpr uint64_t HIGH_STARVATION_DIV      = 7000;
	constexpr uint64_t HIGH_BOOST_MULT          = 12000;

	uint64_t Priority::effective() const
	{
		return this->basePriority
			+ this->temporaryBoost
			+ (getElapsedNanoseconds() - this->lastScheduled) / this->starvationRewardDivisor;
	}

	void Priority::boost(uint64_t amt)
	{
		this->temporaryBoost += this->boostMultiplier * amt;
	}

	void Priority::reset()
	{
		this->lastScheduled = getElapsedNanoseconds();
	}

	Priority Priority::low()
	{
		return Priority {
			.basePriority               = LOW_BASE_PRIORITY,
			.starvationRewardDivisor    = LOW_STARVATION_DIV,
			.boostMultiplier            = LOW_BOOST_MULT
		};
	}

	Priority Priority::normal()
	{
		return Priority {
			.basePriority               = NORMAL_BASE_PRIORITY,
			.starvationRewardDivisor    = NORMAL_STARVATION_DIV,
			.boostMultiplier            = NORMAL_BOOST_MULT
		};
	}

	Priority Priority::high()
	{
		return Priority {
			.basePriority               = HIGH_BASE_PRIORITY,
			.starvationRewardDivisor    = HIGH_STARVATION_DIV,
			.boostMultiplier            = HIGH_BOOST_MULT
		};
	}
}
}
