// sc_misc.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace syscall
{
	void sc_null()
	{
	}

	void sc_exit(int status)
	{
		// ok good. this will yield.
		scheduler::exit(status);
	}

	void sc_log(int level, char* sys, size_t syslen, char* buf, size_t buflen)
	{
		// we should probably handle this somewhere less... dumb?
		switch(level)
		{
			case 0:  serial::debugprint("[log] "); break;
			case 1:  serial::debugprint("[wrn] "); break;
			case 2:  serial::debugprint("[err] "); break;
			default: serial::debugprint("[???] "); break;
		}

		serial::debugprint(sys, syslen);
		serial::debugprint(": ");

		serial::debugprint(buf, buflen);
		serial::debugprint("\n");
	}

	uint64_t sc_nanosecond_timestamp()
	{
		return scheduler::getElapsedNanoseconds();
	}
}
}












