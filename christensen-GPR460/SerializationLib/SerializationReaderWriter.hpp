#pragma once

#include <stdio.h>
#include <type_traits>
#include <vector>
#include <iostream>


#pragma region Endianness handling: Prefer Big Endian

inline bool isLittleEndian()
{
	short word = 0x0001;
	char* wordAsBytes = (char*)&word;
	return wordAsBytes[0];
}

template<typename T, typename std::enable_if<std::is_arithmetic_v<T>>* = nullptr>
void correctEndianness(T* buf) //Symmetric: Both host-to-net and net-to-host
{
	char* bytes = (char*)buf;
	for (int i = 0; i < sizeof(T)/2; ++i) std::swap(bytes[i], bytes[sizeof(T)-1-i]);
}

#pragma endregion


namespace Serialization::Utils
{
	template<typename T>
	static void write(const T& obj, std::ostream& stream) = delete;

	template<typename T>
	static void read(T& obj, std::istream& stream) = delete;

	#pragma region Arithmetics (int, float)

	template<typename T, typename std::enable_if<std::is_arithmetic_v<T>>* = nullptr>
	static void write(T obj, std::ostream& stream) //MUST COPY so we can correct its endianness without modifying original
	{
		correctEndianness(&obj);
		stream.write(&obj, sizeof(T));
	}

	template<typename T, typename std::enable_if<std::is_arithmetic_v<T>>* = nullptr>
	static void read(T& obj, std::istream& stream)
	{
		stream.read(&obj, sizeof(T));
		correctEndianness(&obj);
	}

	#pragma endregion


	#pragma region std::string

	template<>
	static void write(const std::string& obj, std::ostream& stream)
	{
		//Header length
		size_t len = obj.size();
		stream.write((char*) &len, sizeof(size_t));

		//Data
		for (size_t i = 0; i < len; ++i) write(obj.c_str()[i], stream);
	}

	template<>
	static void read(std::string& obj, std::istream& stream)
	{
		//Header length
		size_t len = 0;
		stream.read((char*) &len, sizeof(size_t));

		//Data
		obj.resize(len);
		for (size_t i = 0; i < len; ++i) read(obj.c_str()[i], stream);
	}

	#pragma endregion
}


class SerializationReaderWriter
{
	bool isWriting;
	std::iostream stream;

	template<typename T>
	void serialize(T& obj)
	{
		if (isWriting) Serialization::Utils::write(obj, stream);
		else           Serialization::Utils::read (obj, stream);
	}
};
