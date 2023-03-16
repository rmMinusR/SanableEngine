#pragma once

#include <string>
#include <functional>

#include "SerializationDefines.hpp"

#include "dllapi.h"

class ISerializable;
class EngineCore;

//Should NOT be Trackable, these objects will persist but are not considered a memory leak.
struct SerializationRegistryEntry
{
public:
	const std::string prettyID;
	const binary_id_t binaryID;
	const std::function<void(EngineCore*, std::istream&)> build;

	ENGINESERIALIZATION_API SerializationRegistryEntry(std::string&& prettyID, binary_id_t&& binaryID, std::function<void(EngineCore*, std::istream&)>&& build);
};

//Convenience macro
#define AUTO_SerializationRegistryEntry(TYPE, BUILD) SerializationRegistryEntry(\
	#TYPE, \
	strhash(#TYPE),\
	[](EngineCore* engine, std::istream& in) {\
		BUILD;\
	}\
)