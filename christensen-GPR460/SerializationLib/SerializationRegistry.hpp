#pragma once

#include <vector>
#include <string>

#include "SerializationDefines.hpp"

class ISerializable;
struct SerializationRegistryEntry;

class SerializationRegistry
{
private:
	static SerializationRegistry* INSTANCE;

	SerializationRegistry();

public:
	static SerializationRegistry* getInstance();
	static void cleanupInstance();

	void registerEntry(SerializationRegistryEntry const* entry);
	//SerializationRegistryEntry const* getEntry(const std::string& prettyID);
	SerializationRegistryEntry const* getEntry(const binary_id_t& binaryID);

private:
	std::vector<SerializationRegistryEntry const*> registry;
};