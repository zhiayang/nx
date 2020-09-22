// message.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace ipc
{
	void init()
	{
	}

	void addMessage(const message_t& umsg)
	{
		if(auto proc = scheduler::getProcessWithId(umsg.targetId); proc)
			proc->pendingMessages.lock()->append(umsg);

		else
			warn("ipc", "invalid target '%lu'", umsg.targetId);
	}

	void disposeMessage(message_t& message)
	{
		// there's nothing to do here lmao
	}




	scheduler::Thread* resolveSelector(const selector_t& sel)
	{
		auto path = nx::string_view(sel.sel, sel.len);

		auto parse_id = [](const nx::string_view& sv, bool* succ) -> pid_t {
			char* end = 0;
			auto ret = strtoul(sv.data(), &end, /* base: */ 10);

			if(end == sv.data() || end != (sv.data() + sv.size()))
				*succ = false;

			*succ = true;
			return ret;
		};

		if(path.find("/proc") == 0)
		{
			path.remove_prefix(5);

			if(path.find("/name/") == 0)
			{
				path.remove_prefix(6);
				auto proc = scheduler::getProcessWithName(path.str());

				if(!proc)   return nullptr;
				else        return &proc->threads[0];
			}
			else if(path.find("/id/") == 0)
			{
				path.remove_prefix(4);

				bool succ = false;
				auto id = parse_id(path, &succ);
				if(!succ) return nullptr;

				auto proc = scheduler::getProcessWithId(id);

				if(!proc)   return nullptr;
				else        return &proc->threads[0];
			}
		}
		if(path.find("/thread") == 0)
		{
			path.remove_prefix(7);
			if(path.find("/id/") == 0)
			{
				path.remove_prefix(4);

				bool succ = false;
				auto id = parse_id(path, &succ);
				if(!succ) return nullptr;

				return scheduler::getThreadWithId(id);
			}
		}

		warn("ipc", "failed to resolve selector '%*.s'", path.size(), path.data());
		return nullptr;
	}
}
}


































