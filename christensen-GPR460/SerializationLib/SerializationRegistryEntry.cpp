#include "SerializationRegistryEntry.hpp"

#include "SerializationRegistry.hpp"

SerializationRegistryEntry::SerializationRegistryEntry(std::string&& prettyID, binary_id_t&& binaryID, std::function<void(std::istream&)>&& build) :
	prettyID(prettyID),
	binaryID(binaryID),
	build(build)
{
	SerializationRegistry::getInstance()->registerEntry(this);
}

