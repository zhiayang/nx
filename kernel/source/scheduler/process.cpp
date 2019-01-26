// process.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace scheduler
{
	static Process KernelProcess;
	void setupKernelProcess(addr_t cr3)
	{
		KernelProcess = Process();
		KernelProcess.cr3 = cr3;
	}

	Process* getKernelProcess()
	{
		return &KernelProcess;
	}

	Process* getCurrentProcess()
	{
		return getCurrentCPU()->currentProcess;
	}
}
}























