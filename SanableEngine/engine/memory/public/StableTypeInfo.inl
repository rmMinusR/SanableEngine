#pragma once

#include <type_traits>
#include <string>

#include "dllapi.h"
#include "vtable.h"

struct StableTypeInfo;

struct FieldInfo
{
	std::string name;
	size_t offset;
	StableTypeInfo* type;
};

struct StableTypeInfo
{
	std::string name; //Cannot use typeinfo here
	size_t size = 0;
	vtable_ptr vtable = nullptr;

	std::vector<FieldInfo> fields;

	template<typename TObj>
	void set_vtable(const TObj& obj)
	{
		if (std::is_polymorphic_v<TObj>) vtable = get_vtable_ptr(obj);
		else							 vtable = nullptr;
	}

	//Factories

	template<typename TObj>
	static StableTypeInfo blank()
	{
		StableTypeInfo out;
		out.name = typeid(TObj).name();
		out.size = sizeof(TObj);
		return out;
	}

	template<typename TObj>
	static StableTypeInfo extract(const TObj& obj)
	{
		StableTypeInfo out = blank<TObj>();
		out.vtable = get_vtable_ptr(&obj);
		return out;
	}

	template<typename TObj, typename... TCtorArgs>
	static StableTypeInfo build(TCtorArgs... ctorArgs)
	{
		TObj* obj = new TObj(ctorArgs...);
		StableTypeInfo out = extract(*obj);
		delete obj;
		return out;
	}
};