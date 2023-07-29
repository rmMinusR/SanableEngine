#pragma once

#include <string>
#include <typeinfo>

#pragma region Template helpers

template<typename T, bool isPtr = std::is_pointer<T>::value>
struct PtrUnwrapper
{
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

class TypeName
{
	typedef uint32_t hash_t;
	static hash_t makeHash(const std::string& str);

	std::string unwrappedTypeName;
	hash_t nameHash;
	int ptrDepth;

public:
	TypeName();
	TypeName(const std::string& unwrappedTypeName, int ptrDepth);

	template<typename TRaw>
	static TypeName create()
	{
		return TypeName(
			typeid(typename PtrUnwrapper<TRaw>::RootType).name(),
			PtrUnwrapper<TRaw>::ptrDepth
		);
	}

	//Compare the stuff that's easy before doing a full string compare
	inline bool operator==(const TypeName& other) const { return ptrDepth == other.ptrDepth && nameHash == other.nameHash && unwrappedTypeName == other.unwrappedTypeName; }
	inline bool operator!=(const TypeName& other) const { return ptrDepth != other.ptrDepth || nameHash != other.nameHash || unwrappedTypeName != other.unwrappedTypeName; }

	TypeName(const TypeName& cpy) = default;
	TypeName(TypeName&& mov) = default;
	TypeName& operator=(const TypeName& cpy) = default;
	TypeName& operator=(TypeName&& mov) = default;
	~TypeName() = default;
};
