// options.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "efx.h"
#include "krt.h"

namespace efx {
namespace options
{
	static efx::array<efx::string> loadOptions;

	void parse(const efx::string& opts)
	{
		// ok, we need to construct the global object, because efi does not call our constructors!
		loadOptions = efx::array<efx::string>();

		efi::println("option string: %s", opts.data());

		efx::string currentOption;

		// TODO: quotes!
		bool first = true;
		for(size_t i = 0; i < opts.count(); i++)
		{
			if(opts[i] != ' ')
			{
				currentOption.append(opts[i]);
			}
			else
			{
				// skip the first one, cos it's just our own name
				if(!first) loadOptions.append(currentOption);

				currentOption = "";
				first = false;
			}
		}
		loadOptions.append(currentOption);

		efi::println("parsed %d options:", loadOptions.count());

		int ctr = 0;
		for(const auto& opt : loadOptions)
			efi::println("%d: '%s'", ctr++, opt.data());
	}
}
}



















