#pragma once

#include <string>
#include <typeinfo>

#include "dllapi.h"

#pragma region Template helpers

template<typename T, bool isPtr = std::is_pointer<T>::value>
struct PtrUnwrapper
{
	PtrUnwrapper() = delete;
};

template<typename T>
struct PtrUnwrapper<T, false>
{
	using RootType = typename std::remove_cv<T>::type;
	constexpr static size_t ptrDepth = 0;
};

template<typename T>
struct PtrUnwrapper<T*, true>
{
	using RootType = typename PtrUnwrapper<T>::UnwrappedType;
	constexpr static size_t ptrDepth = PtrUnwrapper<T>::ptrDepth + 1;
};

#pragma endregion

struct TypeInfo;


class TypeName
{
	typedef uint32_t hash_t;
	ENGINEMEM_API static hash_t makeHash(const std::string& str);

	std::string unwrappedTypeName;
	hash_t nameHash;
	size_t ptrDepth;

	friend struct std::hash<TypeName>;

public:
	ENGINEMEM_API TypeName();
	ENGINEMEM_API TypeName(const std::string& unwrappedTypeName, int ptrDepth);

	template<typename TRaw>
	static TypeName create()
	{
		return TypeName(
			typeid(typename PtrUnwrapper<TRaw>::RootType).name(),
			PtrUnwrapper<TRaw>::ptrDepth
		);
	}

	ENGINEMEM_API bool isValid() const; //Whether the name has a valid value. Does NOT indicate whether there is live type data backing it.
	ENGINEMEM_API TypeInfo const* resolve() const;

	ENGINEMEM_API bool operator==(const TypeName& other) const;
	ENGINEMEM_API bool operator!=(const TypeName& other) const;

	ENGINEMEM_API const std::string& as_str() const;
	ENGINEMEM_API char const* c_str() const;

	ENGINEMEM_API TypeName(const TypeName& cpy) = default;
	ENGINEMEM_API TypeName(TypeName&& mov) = default;
	ENGINEMEM_API TypeName& operator=(const TypeName& cpy) = default;
	ENGINEMEM_API TypeName& operator=(TypeName&& mov) = default;
	ENGINEMEM_API ~TypeName() = default;
};


template <>
struct std::hash<TypeName>
{
	std::size_t operator()(const TypeName& k) const
	{
		return k.nameHash;
	}
};
