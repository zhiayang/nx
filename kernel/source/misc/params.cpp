// params.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "misc/params.h"

#define NX_BOOTINFO_VERSION NX_MAX_BOOTINFO_VERSION
#include "bootinfo.h"

namespace nx {
namespace params
{
	struct Option
	{
		Option() { }
		Option(const nx::string& name, const nx::string& value) : name(name), value(value) { }
		Option(nx::string&& name, nx::string&& value) : name(krt::move(name)), value(krt::move(value)) { }

		nx::string name;
		nx::string value;
	};

	static nx::array<Option> Options;

	void init(BootInfo* bi)
	{
		new (&Options) array<Option>();

		if(bi->version < 3)
		{
			warn("kernel", "bootinfo version '%d' does not support kernel parameters", bi->version);
			return;
		}

		char* ptr = bi->kernelParams;
		for(uint64_t i = 0; i < bi->numKernelParams; i++)
		{
			auto str = string(ptr);

			if(auto eqidx = str.find("="); eqidx != (size_t) -1)
			{
				Options.emplace(
					str.substring(0, eqidx),
					str.substring(eqidx + 1)
				);
			}
			else
			{
				Options.emplace(str, "");
			}

			ptr += str.size() + 1;
		}

		log("kernel", "parsed %s", util::plural("kernel parameter", Options.size()).cstr());
	}

	bool haveOption(const nx::string& name)
	{
		for(const auto& opt : Options)
			if(opt.name == name) return true;

		return false;
	}

	nx::string getOptionValue(const nx::string& name)
	{
		for(const auto& opt : Options)
			if(opt.name == name) return opt.value;

		return "";
	}
}
}


















