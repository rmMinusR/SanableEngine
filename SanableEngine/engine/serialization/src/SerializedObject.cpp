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

void debugHex(char const* buf, size_t len) {
	const char* hex = "0123456789ABCDEF";
	for (int i = 0; i < len; ++i) {
		uint8_t byte = buf[i];
		std::cout << hex[byte >> 4] << hex[byte & 0xf] << ' ';
	}
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

	std::cout << typeEntry->prettyID << "[ type=";
	debugHex(reinterpret_cast<char const*>(&typeEntry->binaryID), sizeof(binary_id_t));
	std::cout << "size=";
	debugHex(reinterpret_cast<char const*>(&memberSize), sizeof(size_t));
	std::cout << "] ";
	debugHex(serializedMemberData.c_str(), serializedMemberData.size());
	std::cout << std::endl;

	return true;
}

bool SerializedObject::parse(EngineCore* engine, std::istream& line)
{
	//Read type
	binary_id_t typeBinaryID = 0;
	line.read(reinterpret_cast<char*>(&typeBinaryID), sizeof(binary_id_t));

	//Lookup type in serialization registry
	typeEntry = SerializationRegistry::getInstance()->getEntry(typeBinaryID);

	//Read size
	size_t memberSize = 0;
	line.read(reinterpret_cast<char*>(&memberSize), sizeof(size_t));

	if (typeEntry == nullptr || memberSize == 0) return false;

	//The rest is data
	std::string serializedMemberData;
	serializedMemberData.resize(memberSize, (char)0);
	line.read(&serializedMemberData[0], memberSize);

	//Build
	std::istringstream data(serializedMemberData);
	typeEntry->build(engine, data);

	return true;
}
