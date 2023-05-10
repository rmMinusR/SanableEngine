#include "TypeInfo.hpp"

#include <algorithm>

#include "FieldInfo.hpp"

TypeInfo::TypeInfo(std::string&& shortName, std::string&& absName, std::type_info& stdRtti, size_t size, vtable_ptr vtable, std::initializer_list<FieldInfo> fields)
{
	
}

TypeInfo::~TypeInfo()
{

}

bool TypeInfo::operator==(const TypeInfo& other) const
{
	if (other.hash == this->hash) return true; //Try trivial compare first
	return other.absName == this->absName; //For hot reloads
}

bool TypeInfo::operator!=(const TypeInfo& other) const
{
	return !(*this == other);
}

const std::string& TypeInfo::getShortName() const
{
	return shortName;
}

const std::string& TypeInfo::getAbsName() const
{
	return absName;
}

size_t TypeInfo::getSize() const
{
	return size;
}

const std::vector<FieldInfo>& TypeInfo::getFields() const
{
	return allFields;
}

bool TypeInfo::isPolymorphic() const
{
	return ((int)flags & (int)Flags::POLYMORPHIC) != 0;
}

bool TypeInfo::matches(const std::type_info& type) const
{
	return stdRttiName == type.name();
}

void TypeInfo::vptrJam(void* target) const
{
	char* cursor = (char*)target;
	for (const Parent& i : parents)
	{
		//TODO detect when vtables are combined in multiple inheritance
		//TODO properly handle virtual inheritance
		if (i.type->isPolymorphic())
		{
			i.type->vptrJam(cursor);
			set_vtable_ptr(cursor, i.vtable);
		}
		cursor += i.type->getSize();
	}
}

void TypeInfo::LayoutRemap::execute(void* target)
{
	//TODO implement stub
}

void TypeInfo::LayoutRemap::doSanityCheck()
{
	//TODO implement stub
}

TypeInfo::LayoutRemap TypeInfo::buildLayoutRemap(const TypeInfo* _old, const TypeInfo* _new)
{
	LayoutRemap out;

	//Detect fields that match, and deleted fields
	for (const FieldInfo& oldField : _old->getFields())
	{
		auto newFieldIt = std::find(_new->getFields().cbegin(), _new->getFields().cend(), oldField);
		const FieldInfo* newField = (newFieldIt != _new->getFields().cend()) ? &(*newFieldIt) : nullptr;
		out.contents.emplace_back(&oldField, newField);
	}

	//Detect added fields
	for (const FieldInfo& newField : _new->getFields())
	{
		auto oldFieldIt = std::find(_old->getFields().cbegin(), _old->getFields().cend(), newField);
		if (oldFieldIt == _old->getFields().cend()) out.contents.emplace_back(nullptr, &newField);
	}
	
	return out;
}