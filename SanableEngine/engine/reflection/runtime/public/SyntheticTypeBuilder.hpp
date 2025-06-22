#pragma once

#include <string_view>

#include "TypeInfo.hpp"

class ModuleTypeRegistry;

class SyntheticTypeBuilder
{
	TypeInfo type;
	size_t cursor; //Next free byte
	size_t nextAtAlign(size_t align) const;

	STIX_API void addField_internal(const TypeName& fieldType, std::string_view name, size_t size, size_t align, MemberVisibility visibility);
	STIX_API void addParent_internal(const TypeName& parent, size_t parentSize, size_t parentAlign, MemberVisibility visibility, ParentInfo::Virtualness virtualness);
public:
	STIX_API SyntheticTypeBuilder(const std::string& name);
	STIX_API ~SyntheticTypeBuilder();

	template<typename T>
	void addField(std::string_view name, MemberVisibility visibility = MemberVisibility::Public)
	{
		addField_internal(TypeName::create<T>(), name, sizeof(T), alignof(T), visibility);
	}

	STIX_API void addField(const TypeInfo& type, std::string_view name, MemberVisibility visibility = MemberVisibility::Public);

	template<typename T>
	void addParent(MemberVisibility visibility = MemberVisibility::Public, ParentInfo::Virtualness virtualness = ParentInfo::Virtualness::NonVirtual)
	{
		addParent_internal(TypeName::create<T>(), sizeof(T), alignof(T), visibility, virtualness);
	}

	void addParent(const TypeInfo& type, MemberVisibility visibility = MemberVisibility::Public, ParentInfo::Virtualness virtualness = ParentInfo::Virtualness::NonVirtual)
	{
		addParent_internal(type.name, type.layout.size, type.layout.align, visibility, virtualness);
	}

	STIX_API TypeInfo finalize();
	STIX_API void registerType(ModuleTypeRegistry* reg); //Useful for test mocking
};
