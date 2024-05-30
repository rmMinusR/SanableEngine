#include "GlobalTypeRegistry.hpp"

#include <unordered_set>
#include <cassert>
#include <stdlib.h>

std::unordered_map<GlobalTypeRegistry::module_key_t, ModuleTypeRegistry> GlobalTypeRegistry::modules;
std::optional<GlobalTypeRegistry::Snapshot> GlobalTypeRegistry::_cachedSnapshotValue;

TypeInfo const* GlobalTypeRegistry::lookupType(const TypeName& name)
{
	for (const auto& i : modules)
	{
		TypeInfo const* out = i.second.lookupType(name);
		if (out) return out;
	}
	return nullptr;
}

ModuleTypeRegistry const* GlobalTypeRegistry::getModule(const module_key_t& key)
{
	return &modules.at(key);
}

void GlobalTypeRegistry::loadModule(std::string key, const ModuleTypeRegistry& newTypes)
{
	std::wstring wide(key.length(), ' ');
	std::mbstowcs(wide.data(), key.data(), key.length());
	loadModule(wide, newTypes);
}

void GlobalTypeRegistry::loadModule(module_key_t key, const ModuleTypeRegistry& newTypes)
{
	//If a module already exists with the same name, unload first
	if (modules.find(key) != modules.cend()) unloadModule(key);

	GlobalTypeRegistry::_cachedSnapshotValue = std::nullopt;

	//Register types
	auto it = modules.emplace(key, newTypes).first;
	
	//Finalize late-binding info
	it->second.doLateBinding();
}

TypeInfo const* GlobalTypeRegistry::snipeType(void* obj, size_t size, TypeInfo const* hint)
{
	for (const auto& kv : modules)
	{
		TypeInfo const* out = kv.second.snipeType(obj, size, hint);
		if (out) return out;
	}
	return nullptr;
}

void GlobalTypeRegistry::unloadModule(std::string key)
{
	std::wstring wide(key.length(), ' ');
	std::mbstowcs(wide.data(), key.data(), key.length());
	unloadModule(wide);
}

void GlobalTypeRegistry::unloadModule(module_key_t key)
{
	auto it = modules.find(key);
	assert(it != modules.cend());

	//Unregister types
	modules.erase(it);

	GlobalTypeRegistry::_cachedSnapshotValue = std::nullopt;
}

std::unordered_set<TypeName> GlobalTypeRegistry::getDirtyTypes(const Snapshot& prev)
{
	std::unordered_set<TypeName> out;
	for (const auto& i : prev.hashes) if (!GlobalTypeRegistry::lookupType(i.first)) out.emplace(i.first); //Has been unloaded since snapshot
	for (const auto& m : modules)
	{
		for (const TypeInfo& t : m.second.types)
		{
			if (!prev.hashes.count(t.name)) out.emplace(t.name); //Has been loaded since snapshot
			else if (std::hash<TypeInfo>{}(t) != prev.hashes.at(t.name)) out.emplace(t.name); //Content changed
		}
	}
	return out;
}

void GlobalTypeRegistry::_makeSnapshot_internal()
{
	GlobalTypeRegistry::_cachedSnapshotValue = std::nullopt;
	GlobalTypeRegistry::_cachedSnapshotValue.emplace();
	for (const auto& m : modules)
	{
		for (const TypeInfo& t : m.second.types)
		{
			GlobalTypeRegistry::_cachedSnapshotValue.value().hashes.emplace(t.name, std::hash<TypeInfo>{}(t));
		}
	}
}

GlobalTypeRegistry::Snapshot GlobalTypeRegistry::makeSnapshot()
{
	if (!GlobalTypeRegistry::_cachedSnapshotValue.has_value()) _makeSnapshot_internal();
	return GlobalTypeRegistry::_cachedSnapshotValue.value();
}

GlobalTypeRegistry::Snapshot::Snapshot()
{
}

GlobalTypeRegistry::Snapshot::~Snapshot()
{
}

GlobalTypeRegistry::Snapshot::Snapshot(const Snapshot& cpy)
{
	*this = std::move(cpy);
}

GlobalTypeRegistry::Snapshot::Snapshot(Snapshot&& mov)
{
	*this = std::move(mov);
}

GlobalTypeRegistry::Snapshot& GlobalTypeRegistry::Snapshot::operator=(const Snapshot& cpy)
{
	hashes = cpy.hashes;
	return *this;
}

GlobalTypeRegistry::Snapshot& GlobalTypeRegistry::Snapshot::operator=(Snapshot&& mov)
{
	hashes = std::move(mov.hashes);
	return *this;
}

void GlobalTypeRegistry::clear()
{
	modules.clear();
	_cachedSnapshotValue = std::nullopt;
}
