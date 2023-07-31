#pragma once

#include <unordered_map>
#include <string>

#include "ModuleTypeRegistry.inl"
#include "MemoryMapper.hpp"

/// <summary>
/// Reflection(ish) for all types currently in runtime context
/// </summary>
class GlobalTypeRegistry
{
public:
	typedef std::string module_key_t;

private:
	static std::unordered_map<module_key_t, ModuleTypeRegistry> modules;
	static std::vector<ObjectPatch> pendingPatches;
	
public:
	/// <summary>
	/// Look up a type's metadata, if it is currently alive. If not, returns nullptr.
	/// </summary>
	ENGINEMEM_API static StableTypeInfo const* lookupType(const TypeName& name);

	//////////// INTERNAL FUNCTIONS ////////////

	/// <summary>
	/// Register a new module
	/// </summary>
	ENGINEMEM_API static void addModule(module_key_t key, ModuleTypeRegistry newTypes);

	/// <summary>
	/// Update an existing module, and prepare to patch accordingly
	/// </summary>
	ENGINEMEM_API static void updateModule(module_key_t key, ModuleTypeRegistry newTypes);

	/// <summary>
	/// Get pending patch data. Clears internal state, so call only once!
	/// </summary>
	ENGINEMEM_API [[nodiscard]] static std::vector<ObjectPatch> exportPatch();
};
