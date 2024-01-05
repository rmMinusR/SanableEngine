#pragma once

#include <string>
#include <functional>
#include <optional>

#include "dllapi.h"

#include "ThunkUtils.hpp"
#include "TypeName.hpp"
#include "ParentInfo.hpp"
#include "FieldInfo.hpp"

class TypeBuilder;

/// <summary>
/// For cases where we cannot use C++ builtin type_info.
/// Mainly whenever the owning DLL is unloaded.
/// </summary>
struct TypeInfo
{
	TypeName name;
	size_t size = 0;
	size_t align = 0;

	dtor_t dtor; //Not guaranteed to be present, null check before calling

private:
	std::vector<ParentInfo> parents;
	std::vector<FieldInfo> fields; //NO TOUCHY! Use walkFields instead, which will also handle parent recursion.
	
	/// <summary>
	/// How is each byte used?
	/// </summary>
	enum class ByteUsage : uint8_t
	{
		Unknown = 0,
		ExplicitField = 'f',
		ImplicitConst = 'i',
		Padding = 'p'
	};

	ByteUsage* byteUsage = nullptr; //Each byte maps directly onto implicitValues' corresponding byte
	char* implicitValues = nullptr; //Implicitly generated members (read: vptrs)

	friend class TypeBuilder; //Only thing allowed to touch all member data.

public:
	ENGINE_RTTI_API TypeInfo();
	ENGINE_RTTI_API ~TypeInfo();

	ENGINE_RTTI_API TypeInfo(const TypeInfo& cpy);
	ENGINE_RTTI_API TypeInfo(TypeInfo&& mov);
	ENGINE_RTTI_API TypeInfo& operator=(const TypeInfo& cpy);
	ENGINE_RTTI_API TypeInfo& operator=(TypeInfo&& mov);

	/// <summary>
	/// Check if this type has data (ie. hasn't been empty-constructed).
	/// Does NOT indicate whether using instances will cause errors.
	/// </summary>
	/// <returns></returns>
	ENGINE_RTTI_API bool isValid() const;

	/// <summary>
	/// Check if this type is currently loaded.
	/// If so, instances can be used without causing errors.
	/// </summary>
	ENGINE_RTTI_API bool isLoaded() const;

	/// <summary>
	/// Update from live copy in GlobalTypeRegistry, if one is present.
	/// </summary>
	/// <returns>Whether a live copy was present</returns>
	ENGINE_RTTI_API bool tryRefresh();

	/// <summary>
	/// Look up a field by name. Returns nullptr if not found.
	/// </summary>
	ENGINE_RTTI_API const FieldInfo* getField(const std::string& name,
											MemberVisibility visibilityFlags = MemberVisibility::Public,
											bool includeInherited = true) const;

	/// <summary>
	/// Visit every field in this type matching the given query.
	/// </summary>
	/// <param name="visitor">Function to run on every FieldInfo</param>
	/// <param name="visibilityFlags">What members/parents should be visible or ignored</param>
	/// <param name="includeInherited">Include fields inherited from parents?</param>
	ENGINE_RTTI_API void walkFields(std::function<void(const FieldInfo&)> visitor,
									MemberVisibility visibilityFlags = MemberVisibility::Public,
									bool includeInherited = true) const;

	/// <summary>
	/// Update vtable pointers on the given object instance.
	/// </summary>
	/// <param name="obj">Object to be updated</param>
	ENGINE_RTTI_API void vptrJam(void* obj) const;

	/// <summary>
	/// Cast to a parent. Returns null if no parent found.
	/// </summary>
	/// <param name="obj">Object to cast</param>
	ENGINE_RTTI_API void* upcast(void* obj, const TypeName& name) const;

	/// <summary>
	/// INTERNAL USE ONLY. Currently used to finalize byteUsage, since we need to be able to look up our parents' fields.
	/// </summary>
	ENGINE_RTTI_INTERNAL( void doLateBinding(); )

private:
	/// <summary>
	/// INTERNAL USE ONLY. Currently used to set up byte usage mask.
	/// </summary>
	ENGINE_RTTI_API void create_internalFinalize();

public:
	/// <summary>
	/// INTERNAL USE ONLY by TypeBuilder and TypedMemoryPool.
	/// </summary>
	template<typename TObj>
	static TypeInfo createDummy()
	{
		TypeInfo out;
		out.name = TypeName::create<TObj>();
		out.size = sizeof(TObj);
		out.align = alignof(TObj);
		out.dtor = thunk_utils<TObj>::dtor;
		out.create_internalFinalize();
		return out;
	}
};
