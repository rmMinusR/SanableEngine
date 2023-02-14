#pragma once

#include <string>
#include <functional>

#include "SerializationDefines.hpp"

class ISerializable;

//Should NOT be Trackable, these objects will persist but are not considered a memory leak.
struct SerializationRegistryEntry
{
public:
	const std::string prettyID;
	const binary_id_t binaryID;
	const std::function<void(std::istream&)> build;

	SerializationRegistryEntry(std::string&& prettyID, binary_id_t&& binaryID, std::function<void(std::istream&)>&& build);
};

//Convenience macro
#define AUTO_SerializationRegistryEntry(TYPE, BUILD) SerializationRegistryEntry(\
	#TYPE, \
	strhash(#TYPE),\
	[](std::istream& in) {\
		BUILD;\
	}\
)