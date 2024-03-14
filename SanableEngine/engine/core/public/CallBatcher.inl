#pragma once

#include <algorithm>
#include <vector>

#include "dllapi.h"

template<typename TObj>
class CallBatcher
{
private:
	std::vector<TObj*> objects;

	typedef void** sortID_t; //vptr, if present. TODO: switch to new RTTI system
	static inline sortID_t getSortID(TObj* obj) { return *reinterpret_cast<sortID_t*>(obj); }

public:
	CallBatcher()
	{
		objects.reserve(8);
	}

	template<typename... TArgs>
	void memberCall(void (TObj::*func)(TArgs...), TArgs... funcArgs) const
	{
		for (TObj* o : objects) (o->*func)(funcArgs...);
	}

	template<typename TFunc, typename... TArgs>
	void staticCall(TFunc func, TArgs... funcArgs) const
	{
		for (TObj* o : objects) func(o, funcArgs...);
	}

	void add(TObj* obj)
	{
		auto it = objects.begin();
		while (it != objects.end() && getSortID(obj) > getSortID(*it)) ++it; //Group and order by vptr
		while (it != objects.end() && obj > *it && getSortID(obj) == getSortID(*it)) ++it; //Then order within group by data location
		objects.insert(it, obj);
	}

	void remove(TObj* obj)
	{
		objects.erase(std::find(objects.begin(), objects.end(), obj));
	}

	void clear()
	{
		objects.clear();
	}
};
