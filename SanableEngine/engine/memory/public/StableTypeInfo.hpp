#pragma once

#include <type_traits>
#include <typeinfo>
#include <string>

#include "dllapi.h"
#include "vtable.h"

#include "TypeName.hpp"
#include "MemoryMapper.hpp"

struct StableTypeInfo
{
	TypeName name; //Cannot use C++ builtin type_info here
	size_t size = 0;
	vtable_ptr vtable = nullptr;
	
	bool isValid() const;

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
		out.name = TypeName::create<TObj>();
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
