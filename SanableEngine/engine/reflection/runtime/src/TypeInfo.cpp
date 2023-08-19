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
	//Set own vtable, if present
	if (vtable) set_vtable_ptr(obj, vtable);
	
	//Recurse into parents
	for (const ParentInfo& i : parents)
	{
		i.typeName.resolve()->vptrJam(cast(obj, i.typeName));
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
