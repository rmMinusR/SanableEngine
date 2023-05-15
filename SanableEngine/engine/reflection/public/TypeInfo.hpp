#pragma once

#include <type_traits>
#include <typeinfo>
#include <string>
#include <vector>

#include "dllapi.h"
#include "vtable.h"

class TypeInfo;
struct FieldInfo;

#include "FieldInfo.hpp"

class TypeInfo
{
public:
	struct Parent
	{
		vtable_ptr vtable; //nullptr if not polymorphic
		std::ptrdiff_t memoryOffset;
		TypeInfo* type;
	};

private:
	std::size_t hash; //For fast compares
	std::string stdRttiName; //Obtained from std::type_info/typeid. String so it persists across reloads. Used for matching.

	//Human-friendly names
	std::string shortName;
	std::string absName;

	size_t size = 0;
	enum class Flags
	{
		POLYMORPHIC = 1 << 0
	} flags = (Flags)0;

	std::vector<FieldInfo> ownFields;

	std::vector<Parent> parents;
	std::vector<FieldInfo> allFields; //Includes fields inherited from parents

public:
	ENGINEREFL_API TypeInfo(const std::string& shortName, const std::string& absName, const std::type_info& stdRtti, size_t size, vtable_ptr vtable, std::initializer_list<FieldInfo> fields);
	ENGINEREFL_API ~TypeInfo();

	ENGINEREFL_API const std::string& getShortName() const;
	ENGINEREFL_API const std::string& getAbsName() const;
	ENGINEREFL_API size_t getSize() const;
	ENGINEREFL_API const std::vector<FieldInfo>& getFields() const;
	ENGINEREFL_API bool isPolymorphic() const;

	ENGINEREFL_API bool operator==(const TypeInfo& other) const;
	ENGINEREFL_API bool operator!=(const TypeInfo& other) const;
	ENGINEREFL_API bool matches(const std::type_info&) const;
	template<typename T>
	bool matches() const {
		return matches(typeid(T));
	}

	//Functions for hot reloading
	ENGINEREFL_API void vptrJam(void* target) const;

	struct LayoutRemap
	{
		ENGINEREFL_API void execute(void* obj);
		ENGINEREFL_API void doSanityCheck(); //Complain if new members are introduced, or old members are deleted
	private:
		size_t swapSize; //We need to make a temporary allocation with this algorithm
		std::vector<std::pair<const FieldInfo*, const FieldInfo*>> contents;
		friend class TypeInfo;
	};
	ENGINEREFL_API static LayoutRemap buildLayoutRemap(const TypeInfo* _old, const TypeInfo* _new);
};
