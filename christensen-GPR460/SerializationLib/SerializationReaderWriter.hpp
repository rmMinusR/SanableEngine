#pragma once

#include <iostream>

#include "SerializedTypes.hpp"

class SerializationReaderWriter
{
	bool isWriting;
	std::iostream stream;

	template<typename T>
	void serialize(T& obj)
	{
		if (isWriting) Serialization::Types::write(obj, stream);
		else           Serialization::Types::read (obj, stream);
	}
};
