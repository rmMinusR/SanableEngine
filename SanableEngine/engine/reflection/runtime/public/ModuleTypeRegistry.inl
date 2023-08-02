#pragma once

#include <vector>

#include "TypeInfo.hpp"

class GlobalTypeRegistry;

/// <summary>
/// Reflection(ish) for all types within a module (read: plugin).
/// </summary>
class ModuleTypeRegistry
{
	std::vector<TypeInfo> types;

	friend class TypeBuilder;

public:
	inline TypeInfo const* lookupType(const TypeName& name) const
	{
		for (const TypeInfo& i : types) if (i.name == name) return &i;
		return nullptr;
	}

	inline const std::vector<TypeInfo>& getTypes() const { return types; }
	inline auto cbegin() const { return types.cbegin(); }
	inline auto cend  () const { return types.cend  (); }
};
