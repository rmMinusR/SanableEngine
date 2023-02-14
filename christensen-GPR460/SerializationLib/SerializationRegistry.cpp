#include "SerializationRegistry.hpp"

#include <cassert>

#include "SerializationRegistryEntry.hpp"

SerializationRegistry* SerializationRegistry::INSTANCE = nullptr;

SerializationRegistry::SerializationRegistry() :
	registry()
{
}

SerializationRegistry* SerializationRegistry::getInstance()
{
	if (INSTANCE == nullptr) INSTANCE = new SerializationRegistry();
	return INSTANCE;
}

void SerializationRegistry::cleanupInstance()
{
	delete INSTANCE;
	INSTANCE = nullptr;
}

void SerializationRegistry::registerEntry(SerializationRegistryEntry const* entry)
{
	assert(std::find(registry.begin(), registry.end(), entry) == registry.end());
		
	registry.push_back(entry);
}

SerializationRegistryEntry const* SerializationRegistry::getEntry(const binary_id_t& binaryID)
{
	for (SerializationRegistryEntry const* i : registry)
	{
		if(i->binaryID == binaryID) return i;
	}

	return nullptr;
}

/*
SerializationRegistryEntry const* SerializationRegistry::getEntry(const std::string& prettyID)
{
	for (SerializationRegistryEntry const* i : registry)
	{
		if(i->prettyID == prettyID) return i;
	}

	return nullptr;
}
*/