#pragma once

#include <iostream>

#include "dllapi.h"
#include "SerializationDefines.hpp"

/*

In pretty (aka human readable) mode, reads name to find ctor, then reads rest of line as member data.

In binary mode, reads two bytes as ID to find ctor, then reads one byte to determine size, then reads $size bytes.

*/

struct SerializationRegistryEntry;

class ISerializable
{
public:
	ENGINESERIALIZATION_API virtual ~ISerializable();

protected:
	ENGINESERIALIZATION_API virtual SerializationRegistryEntry const* getRegistryEntry() const = 0;
	//ENGINESERIALIZATION_API virtual void prettySerialize(std::ostream& out) const = 0;
	//ENGINESERIALIZATION_API virtual void prettyDeserialize(std::istream& in) = 0;

	ENGINESERIALIZATION_API virtual void binarySerializeMembers(std::ostream& out) const = 0;
	ENGINESERIALIZATION_API virtual void binaryDeserializeMembers(std::istream& in) = 0;

	friend struct SerializedObject;

	//Helper funcs
	template<typename T>
	void binWriteRaw(const T& val, std::ostream& out) const {
		static_assert(!std::is_pointer_v<T>);

		out.write(reinterpret_cast<char const*>(&val), sizeof(T));
	}

	template<typename T>
	void binReadRaw(T& val, std::istream& in) const {
		static_assert(!std::is_pointer_v<T>);

		in.read(reinterpret_cast<char*>(&val), sizeof(T));
	}
};