#include "TypeInfo.hpp"

#include <cassert>
#include <sstream>

#include "GlobalTypeRegistry.hpp"

TypeInfo::TypeInfo()
{

}

TypeInfo::~TypeInfo()
{
	if (implicitValues)
	{
		free(implicitValues);
		implicitValues = nullptr;
	}

	if (implicitsMask)
	{
		free(implicitsMask);
		implicitsMask = nullptr;
	}
}

TypeInfo::TypeInfo(const TypeInfo& cpy)
{
	*this = cpy; //Defer to operator=
}

TypeInfo::TypeInfo(TypeInfo&& mov)
{
	*this = mov; //Defer to operator=
}

TypeInfo& TypeInfo::operator=(const TypeInfo & cpy)
{
	this->name    = cpy.name;
	this->size    = cpy.size;
	this->dtor    = cpy.dtor;
	this->parents = cpy.parents;
	this->fields  = cpy.fields;

	if (cpy.implicitsMask)
	{
		this->implicitsMask = (char*)malloc(this->size);
		memcpy(this->implicitsMask, cpy.implicitsMask, this->size);
	}
	if (cpy.implicitValues)
	{
		this->implicitValues = (char*)malloc(this->size);
		memcpy(this->implicitValues, cpy.implicitValues, this->size);
	}

	return *this;
}

TypeInfo& TypeInfo::operator=(TypeInfo&& mov)
{
	this->name    = std::move(mov.name);
	this->size    = std::move(mov.size);
	this->dtor    = std::move(mov.dtor);
	this->parents = std::move(mov.parents);
	this->fields  = std::move(mov.fields);

	std::swap(this->implicitsMask , mov.implicitsMask);
	std::swap(this->implicitValues, mov.implicitValues);

	return *this;
}

bool TypeInfo::isValid() const
{
	return size != 0
		&& name.isValid();
}

bool TypeInfo::isLoaded() const
{
	assert(isValid());
	return GlobalTypeRegistry::lookupType(name);
}

bool TypeInfo::tryRefresh()
{
	if (isLoaded())
	{
		//Overwrite values
		*this = *GlobalTypeRegistry::lookupType(name);
		return true;
	}
	else return false;
}

void TypeInfo::walkFields(std::function<void(const FieldInfo&)> visitor, MemberVisibility visibilityFlags, bool includeInherited) const
{
	//Recurse into parents first
	//C++ treats parents as fields placed before the first explicit field
	if (includeInherited)
	{
		for (const ParentInfo& parent : parents)
		{
			if ((int)parent.visibility & (int)visibilityFlags)
			{
				TypeInfo const* parentType = parent.typeName.resolve();
				assert(parentType); //Can't walk what isn't loaded
				parentType->walkFields(
					[&](const FieldInfo& rawField)
					{
						FieldInfo adjustedField = rawField;

						adjustedField.offset += parent.offset;

						std::ostringstream adjustedName;
						adjustedName << parent.typeName.as_str() << "::" << rawField.name;
						adjustedField.name = adjustedName.str();

						visitor(adjustedField);
					},
					visibilityFlags,
					true
				);
			}
		}
	}

	//Walk parents
	for (const FieldInfo& field : fields)
	{
		if ((int)field.visibility & (int)visibilityFlags)
		{
			visitor(field);
		}
	}
}

void TypeInfo::vptrJam(void* obj) const
{
	if (implicitsMask && implicitValues)
	{
		//Write captured constants from implicitly generated fields
		for (size_t i = 0; i < size; ++i)
		{
			if (implicitsMask[i]) static_cast<char*>(obj)[i] = implicitValues[i];
		}
	}
}

void* TypeInfo::cast(void* obj, const TypeName& name) const
{
	//Try casting to direct parent
	for (const ParentInfo& parent : parents)
	{
		if (parent.typeName == name)
		{
			return ((char*)obj) + parent.offset;
		}
	}

	//Try recursing into parents
	for (const ParentInfo& parent : parents)
	{
		void* out = parent.typeName.resolve()->cast(obj, name);
		if (out) return out;
	}

	//Not a parent
	return nullptr;
}

const FieldInfo* TypeInfo::getField(const std::string& name) const
{
	auto it = std::find_if(fields.begin(), fields.end(), [&](const FieldInfo& fi) { return fi.name == name; });
	if (it != fields.end()) return &(*it);
	else return nullptr;
}

void TypeInfo::doLateBinding()
{
	//Deferred from captureCDO: Mark all fields as used
	if (implicitsMask)
	{
		walkFields(
			[=](const FieldInfo& fi) {
				memset(implicitsMask+fi.offset, 0x00, fi.size);
			},
			MemberVisibility::All,
			true
		);
	}
}
