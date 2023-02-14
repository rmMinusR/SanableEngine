#pragma once

typedef uint16_t binary_id_t;

class ISerializable;
class SerializationRegistry;
struct SerializationRegistryEntry;
struct SerializedObject;

//Dan Bernstein's DJB2 algorithm
constexpr binary_id_t strhash(char const* str)
{
	binary_id_t hash = 5381;
	char c = '\0';

	while (c = *str++) hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}