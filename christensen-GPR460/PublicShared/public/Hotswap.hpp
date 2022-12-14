#pragma once

#include "Aliases.hpp"

struct HotswapTypeData
{
	std::string name;
	size_t size;
	vtable_ptr vtable;

	template<typename TObj>
	static std::string extractName() { return typeid(TObj).name(); }

	template<typename TObj, typename... TCtorArgs>
	static HotswapTypeData build(TCtorArgs... ctorArgs)
	{
		TObj* obj = new TObj(ctorArgs...);
		HotswapTypeData out;
		out.vtable = get_vtable_ptr(obj);
		out.name = extractName<TObj>();
		out.size = sizeof(TObj);
		delete obj;
		return out;
	}
};
