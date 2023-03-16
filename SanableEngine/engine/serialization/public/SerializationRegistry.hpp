#pragma once

#include <vector>
#include <string>

#include "SerializationDefines.hpp"
#include "dllapi.h"

class ISerializable;
struct SerializationRegistryEntry;

class SerializationRegistry
{
private:
	ENGINESERIALIZATION_API static SerializationRegistry* INSTANCE;

	ENGINESERIALIZATION_API SerializationRegistry();

public:
	ENGINESERIALIZATION_API static SerializationRegistry* getInstance();
	ENGINESERIALIZATION_API static void cleanupInstance();

	ENGINESERIALIZATION_API void registerEntry(SerializationRegistryEntry const* entry);
	//ENGINESERIALIZATION_API SerializationRegistryEntry const* getEntry(const std::string& prettyID);
	ENGINESERIALIZATION_API SerializationRegistryEntry const* getEntry(const binary_id_t& binaryID);

private:
	std::vector<SerializationRegistryEntry const*> registry;
};