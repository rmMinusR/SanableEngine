#pragma once

#include "ISerializable.hpp"

#include <stdio.h>
#include <cassert>
#include <iostream>

//List-like containers
#include <string>
#include <vector>
#include <list>

//Set-like containers not yet supported
//#include <set>
//#include <unordered_set>
//#include <map>
//#include <unordered_map>

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


struct SerializationOptions
{
	bool binary;

	enum class Target
	{
		Disk,
		Network
	} target;
};


namespace Serialization::Types
{
	template<typename T, typename... TMore> static void write(const T& obj, std::ostream& stream, const SerializationOptions& options) { static_assert(false, "Don't know how to serialize type"  ); }
	template<typename T, typename... TMore> static void read (      T& obj, std::istream& stream, const SerializationOptions& options) { static_assert(false, "Don't know how to deserialize type"); }

	#pragma region Arithmetic types (int, float)

	//Helpers

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

	//Definitions proper

	//We're going to be doing a LOT of definitions. Let's make this a bit easier.
	#define SERIALIZE_ARITHMETIC(type) \
	template<> static void write<type>(const type& obj, std::ostream& stream, const SerializationOptions& options) { writeArithmetic(obj, stream); } \
	template<> static void read <type>(      type& obj, std::istream& stream, const SerializationOptions& options) {  readArithmetic(obj, stream); }

	SERIALIZE_ARITHMETIC(char)
	SERIALIZE_ARITHMETIC(int8_t)
	SERIALIZE_ARITHMETIC(int16_t)
	SERIALIZE_ARITHMETIC(int32_t)
	SERIALIZE_ARITHMETIC(int64_t)
	SERIALIZE_ARITHMETIC(uint8_t)
	SERIALIZE_ARITHMETIC(uint16_t)
	SERIALIZE_ARITHMETIC(uint32_t)
	SERIALIZE_ARITHMETIC(uint64_t)
	SERIALIZE_ARITHMETIC(float)
	SERIALIZE_ARITHMETIC(double)
	SERIALIZE_ARITHMETIC(long double)
	SERIALIZE_ARITHMETIC(bool) //TODO use only one bit when we convert to bitstream

	#pragma endregion

	#pragma region C-arrays

	//Internal use only. DO NOT USE, make users use STL instead
	//Raw pointer has ambiguous ownership, size, and polymorphic-ness
	template<typename T> static void writeArrayUnsafe(T const* arr, const size_t numToWrite, std::ostream& stream, const SerializationOptions& options) { for (size_t i = 0; i < numToWrite; ++i) write(arr[i], stream, options); }
	template<typename T> static void  readArrayUnsafe(T*       arr, const size_t numToRead , std::istream& stream, const SerializationOptions& options) { for (size_t i = 0; i < numToRead ; ++i)  read(arr[i], stream, options); }
	
	template<typename T>
	static void writeArraySafe(T const* arr, const size_t numToWrite, std::ostream& stream, const SerializationOptions& options)
	{
		write(numToWrite, stream, options); //Header: length
		writeArrayUnsafe(arr, numToWrite, stream, options); //Data
	}

	template<typename T>
	static size_t readArraySafe(T* arr, const size_t bufLen, std::istream& stream, const SerializationOptions& options)
	{
		size_t numRead = 0;
		read(numRead, stream, options); //Header: length

		assert(numRead <= bufLen); //Prevent overrun

		readArrayUnsafe(arr, numRead, stream, options); //Data
		
		return numRead;
	}

	#pragma endregion

	#pragma region C-strings

	template<typename T>
	static void writeCString(T const* arr, const size_t numToWrite, std::ostream& stream, const SerializationOptions& options)
	{
		for (; arr != '\0'; ++arr) write(*arr, stream, options);
	}

	template<typename T>
	static size_t readCString(T* arr, const size_t bufLen, std::istream& stream, const SerializationOptions& options)
	{
		size_t numRead = 0;
		read(numRead, stream, options); //Header: length

		assert(numRead <= bufLen); //Prevent overrun

		readArrayUnsafe(arr, numRead, stream, options); //Data
		
		return numRead;
	}

	#pragma endregion

	#pragma region STL ranges and ranged types (std::string, containers)

	template<typename TRange>
	static void writeRange(const TRange& begin, const TRange& end, std::ostream& stream, const SerializationOptions& options)
	{
		write(std::distance(begin, end), stream, options); //Header: length
		for (auto it = begin; it != end; ++it) write(*it, stream, options); //Data
	}

	template<typename TContainer>
	static void writeContainer(const TContainer& container, std::ostream& stream, const SerializationOptions& options)
	{
		writeRange(container.cbegin(), container.cend(), stream, options);
	}

	#pragma region List-like containers (string, vector, list)

	template<typename TContainer>
	static void readListLike(TContainer& container, std::istream& stream, const SerializationOptions& options)
	{
		size_t numRead = 0;
		read(numRead, stream, options); //Header: length

		container.resize(numRead);

		for (auto it = container.begin(); it != container.end(); ++it) read(*it, stream, options); //Data
	}

	#define SERIALIZE_STD_LISTLIKE(containerType) \
	template<typename... Ts> static void write(const containerType<Ts...>& container, std::ostream& stream, const SerializationOptions& options) { writeContainer(container, stream, options); } \
	template<typename... Ts> static void  read(      containerType<Ts...>& container, std::istream& stream, const SerializationOptions& options) {  readListLike (container, stream, options); }
	SERIALIZE_STD_LISTLIKE(std::basic_string )
	SERIALIZE_STD_LISTLIKE(std::vector       )
	SERIALIZE_STD_LISTLIKE(std::list         )

	#pragma endregion

	#pragma region Special cases

	inline static void write(const ISerializable& obj, std::ostream& stream, const SerializationOptions& options) { obj.  serialize(stream, options); }
	inline static void read (      ISerializable& obj, std::istream& stream, const SerializationOptions& options) { obj.deserialize(stream, options); }

	#pragma endregion
};

inline void test()
{
	SerializationOptions options;

	#define __TEST_SER(type) \
	{                        \
		type v;              \
		Serialization::Types::write(v, std::cout, options); \
		Serialization::Types:: read(v, std::cin , options); \
	}
	
	__TEST_SER(char)
	__TEST_SER(int8_t)
	__TEST_SER(int16_t)
	__TEST_SER(int32_t)
	__TEST_SER(int64_t)
	__TEST_SER(uint8_t)
	__TEST_SER(uint16_t)
	__TEST_SER(uint32_t)
	__TEST_SER(uint64_t)
	__TEST_SER(float)
	__TEST_SER(double)
	__TEST_SER(long double)
	__TEST_SER(bool)
	
	__TEST_SER(std::basic_string <char>)
	__TEST_SER(std::vector       <char>)
	__TEST_SER(std::list         <char>)

	//Set-like containers not yet supported
	//__TEST_SER(std::set          <char>)
	//__TEST_SER(std::unordered_set<char>)
	//{
	//	std::map<char, char> v;
	//	Serialization::Types::write(v, std::cout, options);
	//	Serialization::Types:: read(v, std::cin , options);
	//}
	//{
	//	std::unordered_map<char, char> v;
	//	Serialization::Types::write(v, std::cout, options);
	//	Serialization::Types:: read(v, std::cin , options);
	//}
}