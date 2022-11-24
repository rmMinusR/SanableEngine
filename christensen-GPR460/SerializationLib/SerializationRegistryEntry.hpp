#pragma once

#include <string>
#include <functional>

#include "SerializationDefines.hpp"

class ISerializable;

//Should NOT be Trackable, these objects will persist but are not considered a memory leak.
struct SerializationRegistryEntry
{
public:
	const std::string prettyID;
	const binary_id_t binaryID;
	const std::function<ISerializable*()> ctor; //How can we get a blank object, on which to call one of the deserialize functions?
	const std::function<void(ISerializable*)> inject; //When it's time, how does a populated object enter the system?
	const std::function<void(ISerializable*)> dtor; //How should we clean up after ourselves?

	SerializationRegistryEntry(std::string&& prettyID, binary_id_t&& binaryID, std::function<ISerializable*()> ctor, std::function<void(ISerializable*)>&& inject, std::function<void(ISerializable*)>&& dtor);
};

//Convenience macro
#define AUTO_SerializationRegistryEntry(TYPE, SETUP, INJECT, CLEANUP) SerializationRegistryEntry(\
	#TYPE, \
	strhash(#TYPE),\
	[]() {\
		return SETUP;\
	},\
	[](ISerializable*__val) {\
		TYPE* val = static_cast<TYPE*>(__val);\
		INJECT;\
	},\
	[](ISerializable*__val) {\
		TYPE* val = static_cast<TYPE*>(__val);\
		CLEANUP;\
	}\
)