#pragma once

#include <vector>

#include "TypeInfo.hpp"

class GlobalTypeRegistry;

/// <summary>
/// Reflection(ish) for all types within a module (read: DLL/plugin).
/// </summary>
class ModuleTypeRegistry
{
public:
	ENGINE_RTTI_API TypeInfo const* lookupType(const TypeName& name) const;
	ENGINE_RTTI_API const std::vector<TypeInfo>& getTypes() const;
	
private:
	std::vector<TypeInfo> types;
	friend class TypeBuilder;

	void doLateBinding();
	friend class GlobalTypeRegistry;
};
