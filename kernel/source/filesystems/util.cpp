// util.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace vfs
{
	array<string> splitPathComponents(const string& path)
	{
		array<string> ret;

		if(path == "/")
		{
			ret.append("/");
			return ret;
		}
		else
		{
			// skip any leading separators.
			size_t i = 0;
			while(path[i] == '/') i++;

			string current;
			for(; i < path.size(); i++)
			{
				if(path[i] == '/')
				{
					ret.append(current);
					current.clear();
				}
				else
				{
					current.append(path[i]);
				}
			}

			if(current.size() > 0)
				ret.append(current);

			return ret;
		}
	}

	bool isPathSubset(const array<string>& total, const array<string>& subset)
	{
		if(subset.size() > total.size()) return false;

		for(size_t i = 0; i < subset.size(); i++)
		{
			if(subset[i] != total[i])
				return false;
		}

		return true;
	}

	array<string> getFSRelativePath(Filesystem* fs, const array<string>& components)
	{
		auto fsm = splitPathComponents(fs->mountpoint);
		assert(isPathSubset(components, fsm));

		return components.subarray(fsm.size(), -1);
	}

	string sanitise(const string& path)
	{
		auto ret = path;


		return ret;
	}
}
}
























