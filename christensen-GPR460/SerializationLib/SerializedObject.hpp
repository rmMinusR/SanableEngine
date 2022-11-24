#pragma once

#include <string>

#include <Trackable.h>

class ISerializable;
struct SerializationRegistryEntry;

struct SerializedObject : public Trackable
{
public:
	SerializedObject();

	bool serialize(ISerializable const* toSerialize);
	void write(std::ostream& out) const;

	bool parse(std::istream& toDeserialize); //Returns true if the line has parsable data, false if blank/malformed
	bool deserializeAndInject(); //Returns true if we should proceed, false if we should try the line again later

private:
	bool isGood;

	SerializationRegistryEntry const* typeEntry;
	std::string serializedMemberData;
};