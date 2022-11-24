#pragma once

#include <iostream>

#include "SerializationDefines.hpp"

/*

In pretty (aka human readable) mode, reads name to find ctor, then reads rest of line as member data.

In binary mode, reads two bytes as ID to find ctor, then reads one byte to determine size, then reads $size bytes.

*/

struct SerializationRegistryEntry;

class ISerializable
{
public:
	virtual ~ISerializable();

protected:
	virtual SerializationRegistryEntry const* getRegistryEntry() const = 0;
	//virtual void prettySerialize(std::ostream& out) const = 0;
	//virtual void prettyDeserialize(std::istream& in) = 0;

	virtual void binarySerializeMembers(std::ostream& out) const = 0;
	virtual void binaryDeserializeMembers(std::istream& in) = 0;

	friend struct SerializedObject;
};