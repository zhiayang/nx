// options.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "efx.h"
#include "krt.h"

namespace efx {
namespace options
{
	struct option_t
	{
		option_t() { }
		option_t(const efx::string& opt) : option(opt) { }
		option_t(const efx::string& opt, const efx::string& val) : option(opt), value(val) { }

		efx::string option;
		efx::string value;
	};

	static efx::array<option_t> loadOptions;

	void parse(const efx::string& opts)
	{
		// ok, we need to construct the global object, because efi does not call our constructors!
		loadOptions = efx::array<option_t>();
		auto options = efx::array<efx::string>();

		efx::string currentOption;
		efx::stack<char> quoteStack;

		// we skip the first option entirely -- by looking for the first space.
		// this is because (a) it's pointless, and (b) rEFInd uses backslashes, and we want to use those as escape chars.
		size_t start = opts.find(' ');

		// there are actually no options.
		if(start != -1)
		{
			for(size_t i = start + 1; i < opts.size(); i++)
			{
				if(opts[i] == '"' || opts[i] == '\'')
				{
					if(quoteStack.empty() || quoteStack.top() != opts[i])
						quoteStack.push(opts[i]);

					else
						quoteStack.pop();
				}
				else if(opts[i] == '\\')
				{
					// escape something.
					if(i + 1 >= opts.size())
						efi::abort("unexpected end of input after '\\'");

					if(opts[i + 1] == '"')          currentOption.append('"');
					else if(opts[i + 1] == '\'')    currentOption.append('\'');
					else if(opts[i + 1] == 'n')     currentOption.append('\n');
					else
					{
						efi::println("unexpected escape sequence '\\%c', ignoring", opts[i + 1]);
						currentOption.append(opts[i + 1]);
					}

					i++;
				}
				else if(opts[i] != ' ' || !quoteStack.empty())
				{
					currentOption.append(opts[i]);
				}
				else
				{
					options.append(currentOption);
					currentOption = "";
				}
			}
			options.append(currentOption);
		}

		// now we actually need to parse each option.
		for(const auto& opt : options)
		{
			// find an '='
			auto eq = opt.find('=');
			if(eq == -1)
			{
				// ok, just add the option.
				loadOptions.append(option_t(opt, "true"));
			}
			else
			{
				option_t option;
				option.option = opt.substring(0, eq);
				option.value = opt.substring(eq + 1);

				loadOptions.append(option);
			}
		}

		efi::println("parsed %d option%s:", options.size(), options.size() == 1 ? "" : "s");
		for(const auto& opt : loadOptions)
			efi::println("%s = %s", opt.option.cstr(), opt.value.cstr());
	}

	bool has_option(const string& name)
	{
		for(const auto& o : loadOptions)
			if(o.option == name) return true;

		return false;
	}

	string get_option(const string& name)
	{
		for(const auto& o : loadOptions)
			if(o.option == name) return o.value;

		return "";
	}
}
}







































