// util.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "fs.h"

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

			ret.append(current);
			return ret;
		}
	}
}
}
























