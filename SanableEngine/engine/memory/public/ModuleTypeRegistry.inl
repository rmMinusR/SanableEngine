#pragma once

#include <vector>

#include "StableTypeInfo.hpp"
#include "TypedMemoryPool.inl"

class GlobalTypeRegistry;

/// <summary>
/// Reflection(ish) for all types within a module (read: plugin).
/// </summary>
class ModuleTypeRegistry
{
	std::vector<StableTypeInfo> types;

public:
	/// <summary>
	/// Register a type, and auto register the corresponding memory pool's type.
	/// Requires dummy constructor args for a temporary TObj so that its vtable ptr can be extracted.
	/// </summary>
	template<typename TObj, typename... TCtorArgs>
	void registerType(TCtorArgs... ctorArgs)
	{
		types.push_back(StableTypeInfo::build<TObj>(ctorArgs...));
		types.push_back(StableTypeInfo::build<TypedMemoryPool<TObj>>(1));
	}
	
	inline StableTypeInfo const* lookupType(const TypeName& name) const
	{
		for (const StableTypeInfo& i : types) if (i.name == name) return &i;
		return nullptr;
	}

	inline const std::vector<StableTypeInfo>& getTypes() const { return types; }
	inline auto cbegin() const { return types.cbegin(); }
	inline auto cend  () const { return types.cend  (); }
};
