#pragma once

#include <type_traits>

#include "Aliases.hpp"

struct HotswapTypeData
{
	std::string name; //Cannot use typeinfo here
	size_t size = 0;
	vtable_ptr vtable = nullptr;

	template<typename TObj>
	void set_vtable(const TObj& obj)
	{
		if (std::is_polymorphic_v<TObj>) vtable = get_vtable_ptr(obj);
		else							 vtable = nullptr;
	}

	//Factories

	template<typename TObj>
	static HotswapTypeData blank()
	{
		HotswapTypeData out;
		out.name = typeid(TObj).name();
		out.size = sizeof(TObj);
		return out;
	}

	template<typename TObj>
	static HotswapTypeData extract(const TObj& obj)
	{
		HotswapTypeData out = blank<TObj>();
		out.vtable = get_vtable_ptr(&obj);
		return out;
	}

	template<typename TObj, typename... TCtorArgs>
	static HotswapTypeData build(TCtorArgs... ctorArgs)
	{
		TObj* obj = new TObj(ctorArgs...);
		HotswapTypeData out = extract(*obj);
		delete obj;
		return out;
	}
};
