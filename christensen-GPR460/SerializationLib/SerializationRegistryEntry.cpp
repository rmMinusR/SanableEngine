#include "SerializationRegistryEntry.hpp"

#include "SerializationRegistry.hpp"

SerializationRegistryEntry::SerializationRegistryEntry(std::string&& prettyID, binary_id_t&& binaryID, std::function<ISerializable*()> ctor, std::function<void(ISerializable*)>&& inject, std::function<void(ISerializable*)>&& dtor) :
	prettyID(prettyID),
	binaryID(binaryID),
	ctor(ctor),
	inject(inject),
	dtor(dtor)
{
	SerializationRegistry::getInstance()->registerEntry(this);
}