#pragma once

#include "TypeName.hpp"

class CallableMember;

//Pass objects by reference while also erasing their type. Similar to Object in Java/C#/Python.
//Does not own the given object; should never be saved in an object.
class SAnyRef
{
	void* data;
	TypeName type;

	friend class CallableMember;

	ENGINE_RTTI_API void* get_internal(const TypeName& asType) const;
	ENGINE_RTTI_API SAnyRef(void* data, const TypeName& type);
public:
	ENGINE_RTTI_API SAnyRef();
	ENGINE_RTTI_API ~SAnyRef();

	template<typename T>
	static SAnyRef make(T* obj) { return SAnyRef(obj, TypeName::create<T>()); } //TODO attempt to snipe RTTI and checK, just to be sure? Casting should handle most of it though.

	ENGINE_RTTI_API TypeName getType() const;
	template<typename T>
	T* get() const { return get_internal(TypeName::create<T>()); }

	ENGINE_RTTI_API operator bool() const;
	ENGINE_RTTI_API bool has_value() const;

	//Allow copy and move
	SAnyRef(const SAnyRef& cpy) = default;
	SAnyRef& operator=(const SAnyRef& cpy) = default;
	SAnyRef(SAnyRef&& mov) = default;
	SAnyRef& operator=(SAnyRef&& mov) = default;
};
