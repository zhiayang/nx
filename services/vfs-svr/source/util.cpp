// util.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "internal.h"

#include <assert.h>

#include <vector>
#include <string>

namespace vfs
{
	extern VfsState state;


	std::vector<std::string> splitPathComponents(std::string_view path)
	{
		std::vector<std::string> ret;

		if(path == "/")
		{
			ret.push_back("/");
			return ret;
		}
		else
		{
			// skip any leading separators.
			size_t i = 0;
			while(path[i] == '/') i++;

			std::string current;
			for(; i < path.size(); i++)
			{
				if(path[i] == '/')
				{
					ret.push_back(current);
					current.clear();
				}
				else
				{
					current.push_back(path[i]);
				}
			}

			if(current.size() > 0)
				ret.push_back(current);

			return ret;
		}
	}

	bool isPathSubset(const std::vector<std::string>& total, const std::vector<std::string>& subset)
	{
		if(subset.size() > total.size()) return false;

		for(size_t i = 0; i < subset.size(); i++)
		{
			if(subset[i] != total[i])
				return false;
		}

		return true;
	}

	std::vector<std::string> getFSRelativePath(Filesystem* fs, const std::vector<std::string>& components)
	{
		auto fsm = splitPathComponents(fs->mountpoint);
		assert(isPathSubset(components, fsm));

		return std::vector<std::string>(components.begin() + fsm.size(), components.end());
	}

	std::string concatPath(const std::vector<std::string>& components)
	{
		std::string ret;
		for(const auto& p : components)
			ret += "/" + p;

		return ret;
	}

	std::string sanitise(std::string_view path)
	{
		auto ret = path;
		return std::string(ret);
	}

	Filesystem* getFilesystemForPath(std::string_view path)
	{
		auto xs = splitPathComponents(path);
		for(auto& fs : state.mountedFilesystems)
		{
			auto ys = splitPathComponents(fs.mountpoint);
			if(isPathSubset(xs, ys))
				return &fs;
		}

		return nullptr;
	}
}


