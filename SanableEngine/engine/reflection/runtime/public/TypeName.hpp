#pragma once

#include <string>
#include <typeinfo>

#include "dllapi.h"

#pragma region Template helpers

template<typename T>
struct PtrUnwrapper
{
	using RootType = typename std::remove_cv<T>::type;
	constexpr static size_t ptrDepth = 0;
};

template<typename T>
struct PtrUnwrapper<T*>
{
	using RootType = typename PtrUnwrapper<T>::RootType;
	constexpr static size_t ptrDepth = PtrUnwrapper<T>::ptrDepth + 1;
};

#pragma endregion

struct TypeInfo;


class TypeName
{
	typedef uint32_t hash_t;
	ENGINE_RTTI_API static hash_t makeHash(const std::string& str);

	std::string unwrappedTypeName;
	hash_t nameHash;
	size_t ptrDepth;

	friend struct std::hash<TypeName>;

public:
	ENGINE_RTTI_API TypeName();
	ENGINE_RTTI_API TypeName(const std::string& unwrappedTypeName); //Convenience ctor when not dealing with ptrs
	ENGINE_RTTI_API TypeName(const std::string& unwrappedTypeName, int ptrDepth);

	ENGINE_RTTI_API static TypeName parse(const std::string& unsafeName);

	template<typename TRaw>
	static TypeName create()
	{
		return TypeName(
			typeid(TRaw).name(),
			PtrUnwrapper<TRaw>::ptrDepth
		);
		/*
		return TypeName(
			typeid(typename PtrUnwrapper<TRaw>::RootType).name(),
			PtrUnwrapper<TRaw>::ptrDepth
		);
		*/
	}

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
