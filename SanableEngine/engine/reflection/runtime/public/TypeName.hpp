#pragma once

#include <string>
#include <typeinfo>
#include <optional>
#include <vector>

#include "dllapi.h"

struct TypeInfo;

class TypeName
{
private:
	//Strip functions return true if any changes were made to str
	static bool strip(std::string& str, const std::string& phrase, bool spacePadded);
	static bool strip_leading(std::string& str, const std::string& phrase);
	static bool strip_trailing(std::string& str, const std::string& phrase);

	typedef uint32_t hash_t;
	static hash_t makeHash(const std::string& str);

	std::string name;
	hash_t nameHash;
	friend struct std::hash<TypeName>;

	ENGINE_RTTI_API TypeName(const std::string& name);
public:
	ENGINE_RTTI_API TypeName();

	template<typename TRaw>
	static TypeName create()
	{
		return TypeName(typeid(TRaw).name());
	}

	template<typename... TPack>
	static std::vector<TypeName> createPack()
	{
		std::vector<TypeName> out;
		out.reserve(sizeof...(TPack));
		createPack_internal<0, TPack...>(out);
		return out;
	}
private:
	template<int, typename Head, typename... Tail>
	static void createPack_internal(std::vector<TypeName>& out) { out.push_back(create<Head>()); createPack_internal<0, Tail...>(out); }
	template<int> static void createPack_internal(std::vector<TypeName>& out) { } //Tail case

public:
	ENGINE_RTTI_API std::optional<TypeName> cvUnwrap() const;
	ENGINE_RTTI_API std::optional<TypeName> dereference() const;

	ENGINE_RTTI_API bool isValid() const; //Whether the name has a valid value. Does NOT indicate whether there is live type data backing it.
	ENGINE_RTTI_API TypeInfo const* resolve() const;

	ENGINE_RTTI_API bool operator==(const TypeName& other) const;
	ENGINE_RTTI_API bool operator!=(const TypeName& other) const;

	ENGINE_RTTI_API const std::string& as_str() const;
	ENGINE_RTTI_API char const* c_str() const;

	ENGINE_RTTI_API TypeName(const TypeName& cpy) = default;
	ENGINE_RTTI_API TypeName(TypeName&& mov) = default;
	ENGINE_RTTI_API TypeName& operator=(const TypeName& cpy) = default;
	ENGINE_RTTI_API TypeName& operator=(TypeName&& mov) = default;
	ENGINE_RTTI_API ~TypeName() = default;
};


template <>
struct std::hash<TypeName>
{
	ENGINE_RTTI_API std::size_t operator()(const TypeName& k) const
	{
		return k.nameHash;
	}
};
