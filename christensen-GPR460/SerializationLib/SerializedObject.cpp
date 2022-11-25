#include "SerializedObject.hpp"

#include <cassert>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "ISerializable.hpp"
#include "SerializationRegistry.hpp"
#include "SerializationRegistryEntry.hpp"

SerializedObject::SerializedObject() :
	typeEntry(nullptr)
{
}

bool SerializedObject::serialize(ISerializable const* toSerialize, std::ostream& out)
{
	//Retrieve type entry
	typeEntry = toSerialize->getRegistryEntry();
	assert(typeEntry);

	std::ostringstream memberOut(std::ios::binary);
	/*if (isBinary)*/ toSerialize->binarySerializeMembers(memberOut);
	//else          toSerialize->prettySerialize(out);
	std::string serializedMemberData = memberOut.str();

	//Write type ID string
	out.write(reinterpret_cast<char const*>(&typeEntry->binaryID), sizeof(binary_id_t));

	//Write size
	size_t memberSize = serializedMemberData.length();
	out.write(reinterpret_cast<char*>(&memberSize), sizeof(size_t));

	//Write members
	out.write(serializedMemberData.c_str(), serializedMemberData.size());

	return true;
}

bool SerializedObject::parse(std::istream& line)
{
	//Read type
	binary_id_t typeBinaryID = 0;
	line.read(reinterpret_cast<char*>(&typeBinaryID), sizeof(binary_id_t));

	//Lookup type in serialization registry
	typeEntry = SerializationRegistry::getInstance()->getEntry(typeBinaryID);

	//Read size
	size_t memberSize = 0;
	line.read(reinterpret_cast<char*>(&memberSize), sizeof(size_t));
	assert(memberSize != 0);

	if (typeEntry == nullptr) return false;

	//The rest is data
	std::string serializedMemberData;
	serializedMemberData.resize(memberSize, (char)0);
	line.read(&serializedMemberData[0], memberSize);

	//Build
	std::istringstream data(serializedMemberData);
	typeEntry->build(data);

	return true;
}
