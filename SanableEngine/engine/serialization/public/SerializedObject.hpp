#pragma once

#include <string>

#include "dllapi.h"

class ISerializable;
class EngineCore;
struct SerializationRegistryEntry;

struct SerializedObject
{
public:
	ENGINESERIALIZATION_API SerializedObject();

	ENGINESERIALIZATION_API bool serialize(ISerializable const* toSerialize, std::ostream& out);
	ENGINESERIALIZATION_API bool parse(EngineCore* engine, std::istream& toDeserialize); //Returns true if the entry was successfully parsed, false if malformed
private:
	SerializationRegistryEntry const* typeEntry;
};