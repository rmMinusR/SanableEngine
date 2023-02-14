#pragma once

#include <string>

class ISerializable;
struct SerializationRegistryEntry;

struct SerializedObject
{
public:
	SerializedObject();

	bool serialize(ISerializable const* toSerialize, std::ostream& out);
	bool parse(std::istream& toDeserialize); //Returns true if the entry was successfully parsed, false if malformed
private:
	SerializationRegistryEntry const* typeEntry;
};