#pragma once

#include <algorithm>

#include "dllapi.h"
#include "rttiutils.h"

template<typename TObj>
class CallBatcher
{
private:
	std::vector<TObj*> objects;

	typedef vtable_ptr sortID_t;
	static inline sortID_t getSortID(TObj* obj) { return get_vtable_ptr(obj); }

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
		size_t insertIndex = 0;
		while (insertIndex < objects.size() && getSortID(obj) > getSortID(objects[insertIndex])) insertIndex++;
		objects.insert(objects.begin()+insertIndex, obj);
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
