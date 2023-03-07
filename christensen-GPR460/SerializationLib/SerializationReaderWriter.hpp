#pragma once

#include <stdio.h>
#include <type_traits>
#include <vector>
#include <iostream>
#include <cassert>

#include "ISerializable.hpp"


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
	if (isLittleEndian())
	{
		char* bytes = (char*)buf;
		for (int i = 0; i < sizeof(T)/2; ++i) std::swap(bytes[i], bytes[sizeof(T)-1-i]);
	}
}

#pragma endregion


class SerializationReaderWriter
{
	bool isWriting;
	std::iostream stream;

	template<typename T>
	void serialize(T& obj)
	{
		if (isWriting) write(obj, stream);
		else           read (obj, stream);
	}

private:
	template<typename T> static void write(const T& obj, std::ostream& stream) { static_assert(false, "Don't know how to serialize type"); };
	template<typename T> static void read (      T& obj, std::istream& stream) { static_assert(false, "Don't know how to deserialize type"); };

	#pragma region ISerializable

	template<>
	static void write<ISerializable>(const ISerializable& obj, std::ostream& stream)
	{
		obj.serializeMembers(stream);
	}

	template<>
	static void read<ISerializable>(ISerializable& obj, std::istream& stream)
	{
		obj.deserializeMembers(stream);
	}

	#pragma endregion

	#pragma region Arithmetics (int, float)

	template<typename T>
	static void writeArithmetic(const T& obj, std::ostream& stream)
	{
		static_assert(std::is_arithmetic_v<T> || std::is_enum_v<T>);
		T buf = obj;
		correctEndianness(&buf);
		stream.write((char*)&buf, sizeof(T));
	}

	template<typename T>
	static void readArithmetic(T& obj, std::istream& stream)
	{
		static_assert(std::is_arithmetic_v<T> || std::is_enum_v<T>);
		stream.read((char*)&obj, sizeof(T));
		correctEndianness(&obj);
	}

	#pragma endregion

	#pragma region C-arrays and C-strings

	template<typename T>
	static void writeArrayUnsafe(T const* arr, const size_t numToWrite, std::ostream& stream)
	{
		for (size_t i = 0; i < numToWrite; ++i) write(arr[i], stream);
	}

	template<typename T>
	static void readArrayUnsafe(T* arr, const size_t numToRead, std::istream& stream)
	{
		for (size_t i = 0; i < numToRead; ++i) read(arr[i], stream);
	}

	template<typename T>
	static void writeArraySafe(T const* arr, const size_t numToWrite, std::ostream& stream)
	{
		writeArithmetic(numToWrite, stream); //Header length
		writeArrayUnsafe(arr, numToWrite, stream); //Data
	}

	template<typename T>
	static size_t readArraySafe(T* arr, const size_t bufLen, std::istream& stream)
	{
		size_t numRead = 0;
		readArithmetic(numRead, stream); //Header length

		assert(numRead <= bufLen); //Prevent overrun
		readArrayUnsafe(arr, numRead, stream); //Data
		
		return numRead;
	}

	#pragma endregion

	#pragma region std::string and STL containers

	static void writeString(const std::string& str, std::ostream& stream)
	{
		writeArraySafe(str.c_str(), str.size(), stream);
	}

	static void readString(std::string& obj, std::istream& stream)
	{
		size_t numRead = 0;
		readArithmetic(numRead, stream); //Header length

		obj.resize(numRead);
		readArrayUnsafe(obj.c_str(), numRead, stream); //Data
	}

	template<typename T>
	static void writeSTL(const T& container, std::ostream& stream)
	{
		writeArraySafe(container.data(), container.size(), stream);
	}

	template<typename T>
	static void readSTL(T& container, std::istream& stream)
	{
		size_t numRead = 0;
		readArithmetic(numRead, stream); //Header length
		
		container.resize(numRead);
		readArrayUnsafe(container.data(), numRead, stream); //Data
	}

	#pragma endregion
};
