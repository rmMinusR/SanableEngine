#include "SerializedObject.hpp"

#include <cassert>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "ISerializable.hpp"
#include "SerializationRegistry.hpp"
#include "SerializationRegistryEntry.hpp"

SerializedObject::SerializedObject() :
	typeEntry(nullptr),
	isGood(false)
{
}

bool SerializedObject::serialize(ISerializable const* toSerialize)
{
	isGood = false;

	//Retrieve type entry
	typeEntry = toSerialize->getRegistryEntry();
	assert(typeEntry);

	std::ostringstream out(std::ios::binary);
	/*if (isBinary)*/ toSerialize->binarySerializeMembers(out);
	//else          toSerialize->prettySerialize(out);
	serializedMemberData = out.str();

	isGood = true;
	return true;
}

void SerializedObject::write(std::ostream& out) const
{
	assert(isGood);

	//Write type ID string
	out.write(reinterpret_cast<char const*>(&typeEntry->binaryID), sizeof(binary_id_t));

	//Write size
	size_t memberSize = serializedMemberData.length();
	out.write(reinterpret_cast<char*>(&memberSize), sizeof(size_t));

	//std::cout << typeEntry->prettyID << " wrote " << memberSize << "bytes of members (+" << (sizeof(binary_id_t) + sizeof(size_t)) << " overhead)" << std::endl;

	//Write members
	out << serializedMemberData;
}

bool SerializedObject::parse(std::istream& line)
{
	//Read type
	binary_id_t typeBinaryID = 0;
	line.read(reinterpret_cast<char*>(&typeBinaryID), sizeof(binary_id_t)); //Is this write legal? Do we need to offset in the other direction?
	if (typeBinaryID == 0) {
		return false;
		//assert(typeBinaryID != 0);
	}

	//Lookup type in serialization registry
	typeEntry = SerializationRegistry::getInstance()->getEntry(typeBinaryID);

	//Read size
	size_t memberSize = UINT_MAX;
	line.read(reinterpret_cast<char*>(&memberSize), sizeof(size_t));
	assert(memberSize != UINT_MAX);

	//std::cout << typeEntry->prettyID << " parsed " << memberSize << " bytes of members (+" << (sizeof(binary_id_t) + sizeof(size_t)) << " overhead)" << std::endl;

	//The rest is data
	serializedMemberData.resize(memberSize, (char)0xCC);
	line.read(&serializedMemberData[0], memberSize);

	isGood = typeEntry != nullptr;
	return isGood;
}

bool SerializedObject::deserializeAndInject()
{
	assert(isGood);

	//std::cout << "Deserializing and injecting " << typeEntry->prettyID << std::endl;

	//Instantiate
	ISerializable* deserialized = typeEntry->ctor();

	//Deserialize
	std::istringstream data(serializedMemberData);
	/*if (isBinary)*/ deserialized->binaryDeserializeMembers(data);
	//else          deserialized->prettyDeserialize(data);

	//Dispatch
	typeEntry->inject(deserialized);

	//Delete
	typeEntry->dtor(deserialized);

	return true;
}
