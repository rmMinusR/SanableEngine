#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>

#include "ModuleTypeRegistry.hpp"

/// <summary>
/// Reflection(ish) for all types currently in runtime context
/// </summary>
class GlobalTypeRegistry
{
public:
	typedef std::string module_key_t;

private:
	static std::unordered_map<module_key_t, ModuleTypeRegistry> modules;
	static std::unordered_set<TypeName> dirtyTypes;
	
public:
	/// <summary>
	/// Look up a type's metadata, if it is currently alive. If not, returns nullptr.
	/// </summary>
	ENGINE_RTTI_API static TypeInfo const* lookupType(const TypeName& name);

	//////////// INTERNAL FUNCTIONS ////////////

	/// <summary>
	/// Register a new module. If one already exists with the same name, it will be unloaded first.
	/// </summary>
	ENGINE_RTTI_API static void loadModule(module_key_t key, const ModuleTypeRegistry& newTypes);

	/// <summary>
	/// Mark a module as unloaded
	/// </summary>
	ENGINE_RTTI_API static void unloadModule(module_key_t key);

	/// <summary>
	/// Get the names of any types that have been altered via updateModule since last call.
	/// </summary>
	[[nodiscard]] ENGINE_RTTI_API static std::unordered_set<TypeName> getDirtyTypes();

	ENGINE_RTTI_API static void clear();
};
