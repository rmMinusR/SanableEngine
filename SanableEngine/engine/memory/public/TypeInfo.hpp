#pragma once

#include <type_traits>
#include <typeinfo>
#include <string>
#include <functional>

#include "dllapi.h"
#include "vtable.h"

#include "MemberInfo.hpp"
#include "MemoryMapper.hpp"

#pragma region Destructor type erasure

typedef void (*dtor_t)(void*); //CANNOT be a std::function or lambda because destroying the dtor_t instance would attempt to delete memory from a possibly-unloaded plugin

template<typename T, bool has_destructor = std::is_destructible<T>::value>
struct dtor_utils
{
	dtor_utils() = delete;
};

template<typename T>
struct dtor_utils<T, true>
{
	inline static void call_dtor(void* obj)
	{
		//C++ forbids getting the address of a dtor, but we can still wrap it
		static_cast<const T*>(obj)->~T();
	}

	constexpr static dtor_t dtor = &call_dtor;
};

template<typename T>
struct dtor_utils<T, false>
{
	//Can't call a dtor that doesn't exist
	constexpr static dtor_t dtor = nullptr;
};

#pragma endregion

class TypeBuilder;


/// <summary>
/// For cases where we cannot use C++ builtin type_info.
/// Mainly whenever the owning DLL is unloaded.
/// </summary>
struct TypeInfo
{
	TypeName name;
	size_t size = 0;

	dtor_t dtor; //Not guaranteed to be present, null check before calling

	std::vector<ParentInfo> parents;

private:
	std::vector<FieldInfo> fields; //NO TOUCHY! Use walkFields instead, which will also handle parent recursion.
	std::vector<VTableInfo> vtables; //EXTREME NO TOUCHY! Use vptrJam instead.

	friend class TypeBuilder; //Only thing allowed to touch all member data.

public:
	ENGINEMEM_API TypeInfo() = default;
	ENGINEMEM_API ~TypeInfo() = default;

	/// <summary>
	/// Check if this type has data (ie. hasn't been empty-constructed).
	/// Does NOT indicate whether using instances will cause errors.
	/// </summary>
	/// <returns></returns>
	ENGINEMEM_API bool isValid() const;

	/// <summary>
	/// Check if this type is currently loaded.
	/// If so, instances can be used without causing errors.
	/// </summary>
	ENGINEMEM_API bool isLoaded() const;

	/// <summary>
	/// Update from live copy in GlobalTypeRegistry, if one is present.
	/// </summary>
	/// <returns>Whether a live copy was present</returns>
	ENGINEMEM_API bool tryRefresh();

	/// <summary>
	/// Visit every field in this type matching the given query.
	/// </summary>
	/// <param name="visitor">Function to run on every FieldInfo</param>
	/// <param name="visibilityFlags">What members/parents should be visible or ignored</param>
	/// <param name="includeInherited">Include fields inherited from parents?</param>
	ENGINEMEM_API void walkFields(std::function<void(const FieldInfo&)> visitor,
								  MemberVisibility visibilityFlags = MemberVisibility::Public,
								  bool includeInherited = true) const;

	/// <summary>
	/// Update vtable pointers on the given object instance.
	/// </summary>
	/// <param name="obj"></param>
	ENGINEMEM_API void vptrJam(void* obj) const;

	template<typename TObj>
	void set_vtable(const TObj& obj)
	{
		if (std::is_polymorphic_v<TObj>) vtable = get_vtable_ptr(obj);
		else							 vtable = nullptr;
	}

	//Factories

	template<typename TObj>
	static TypeInfo blank()
	{
		TypeInfo out;
		out.name = TypeName::create<TObj>();
		out.size = sizeof(TObj);
		out.dtor = dtor_utils<TObj>::dtor;
		return out;
	}

	template<typename TObj>
	static TypeInfo extract(const TObj& obj) //Requires EXACT type
	{
		TypeInfo out = blank<TObj>();
		out.vtable = get_vtable_ptr(&obj);
		return out;
	}

	template<typename TObj, typename... TCtorArgs>
	static TypeInfo build(TCtorArgs... ctorArgs)
	{
		TObj* obj = new TObj(ctorArgs...);
		TypeInfo out = extract(*obj);
		delete obj;
		return out;
	}
};
