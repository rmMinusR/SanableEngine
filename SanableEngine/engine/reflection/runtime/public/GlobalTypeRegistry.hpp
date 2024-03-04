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
	/// Attempt to find the exact type of a void pointer
	/// </summary>
	/// <param name="obj">Object of unknown type</param>
	/// <param name="size">Size of object. Can be semi-inaccurate as long as we can safely read memory in the specified range.</param>
	/// <param name="hint">Parent type, can be null if unknown</param>
	/// <returns>Pointer to matching type, or null if none was found</returns>
	ENGINE_RTTI_API static TypeInfo const* snipeType(void* obj, size_t size, TypeInfo const* hint = nullptr);

	/// <summary>
	/// Get the names of any types that have been altered via updateModule since last call.
	/// </summary>
	[[nodiscard]] ENGINE_RTTI_API static std::unordered_set<TypeName> getDirtyTypes();

	/// <summary>
	/// Completely wipe internal state. Useful for shutdown cleanup and unit testing.
	/// </summary>
	ENGINE_RTTI_API static void clear();
};
