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

	void doLateBinding();
	friend class GlobalTypeRegistry;
public:
	TypeInfo const* lookupType(const TypeName& name) const;
	const std::vector<TypeInfo>& getTypes() const;
};
