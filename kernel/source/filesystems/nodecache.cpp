// nodecache.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "krt.h"

namespace nx {
namespace vfs
{
	Node* nodecache::fetch(const nx::string& path)
	{
		auto comps = splitPathComponents(path);

		if(this->store.size() < comps.size())
			return nullptr;

		Node* ret = 0;
		for(size_t i = 0; i < comps.size(); i++)
		{
			auto& map = this->store[i];
			if(auto it = map.find(comps[i]); it != map.end())
				ret = it->value;

			else
				return nullptr;
		}

		return ret;
	}

	void nodecache::insert(const nx::string& path, Node* node)
	{
		auto comps = splitPathComponents(path);

		// pad to size
		auto d = comps.size() - this->store.size();
		for(size_t i = 0; i < d; i++)
			this->store.append(treemap<string, Node*>());

		treemap<string, Node*>* map = 0;
		for(size_t i = 0; i < comps.size(); i++)
			map = &this->store[i];

		bool ret = map->insert(path, node);

		if(!ret) abort("something already exists at path!");
	}

	void nodecache::evict(const nx::string& path)
	{
		auto comps = splitPathComponents(path);
		assert(this->store.size() >= comps.size());

		treemap<string, Node*>* map = 0;
		for(size_t i = 0; i < comps.size(); i++)
			map = &this->store[i];

		map->remove(comps.back());
	}

	size_t nodecache::size()
	{
		return this->count;
	}

	void nodecache::clear()
	{
		this->store.clear();
	}
}
}



























