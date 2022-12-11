#pragma once

template<typename TObj>
class CallBatcher
{
private:
	std::vector<TObj*> objects;

	typedef void* sortID_t; //We don't know the type of a vtable ptr
	static inline sortID_t getSortID(TObj* obj)
	{
		//VC++ puts the vtable ptr at the very start of the object's memory
		return *reinterpret_cast<sortID_t*>(obj);
	}

public:
	template<typename... TArgs>
	void memberCall(void (TObj::*func)(TArgs...), TArgs... funcArgs)
	{
		for (TObj* o : objects) (o->*func)(funcArgs...);
	}

	template<typename TFunc, typename... TArgs>
	void staticCall(void *(*func)(TArgs...), TArgs... funcArgs)
	{
		for (TObj* o : objects) (*func)(o, funcArgs...);
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
};
