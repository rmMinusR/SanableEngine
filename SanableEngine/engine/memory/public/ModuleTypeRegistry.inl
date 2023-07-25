#pragma once

#include <vector>

#include "StableTypeInfo.inl"
#include "TypedMemoryPool.inl"

class ModuleTypeRegistry
{
	std::vector<StableTypeInfo> types;

public:
	/// <summary>
	/// Register a type, and auto register the corresponding memory pool's type.
	/// </summary>
	template<typename TObj, typename... TCtorArgs>
	void registerType(TCtorArgs... ctorArgs)
	{
		types.push_back(StableTypeInfo::build<TObj>(ctorArgs...));
		types.push_back(StableTypeInfo::build<TypedMemoryPool<TObj>>(1));
	}
	
	inline const std::vector<StableTypeInfo>& getTypes() { return types; }
	inline auto cbegin() const { return types.cbegin(); }
	inline auto cend  () const { return types.cend  (); }
};
