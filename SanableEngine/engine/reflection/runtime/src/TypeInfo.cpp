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

	if (byteUsage)
	{
		free(byteUsage);
		byteUsage = nullptr;
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

	if (cpy.byteUsage)
	{
		this->byteUsage = (ByteUsage*)malloc(this->size);
		memcpy(this->byteUsage, cpy.byteUsage, this->size);
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

	std::swap(this->byteUsage     , mov.byteUsage     );
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

const FieldInfo* TypeInfo::getField(const std::string& name, MemberVisibility visibilityFlags, bool includeInherited) const
{
	//Search own fields
	auto it = std::find_if(fields.begin(), fields.end(), [&](const FieldInfo& fi) { return fi.name == name; });
	if (it != fields.end() && ((int)it->visibility & (int)visibilityFlags)) return &(*it);
	
	//Search parent fields
	if (includeInherited)
	{
		for (const ParentInfo& parent : parents)
		{
			if ((int)parent.visibility & (int)visibilityFlags)
			{
				const FieldInfo* out = parent.typeName.resolve()->getField(name, visibilityFlags, true);
				if (out) return out;
			}
		}
	}

	//Found nothing
	return nullptr;
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
				const TypeInfo* parentType = parent.typeName.resolve();
				assert(parentType); //Can't walk what isn't loaded
				parentType->walkFields(
					visitor,
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
	assert(byteUsage);
	if (implicitValues)
	{
		//Write captured constants from implicitly generated fields
		for (size_t i = 0; i < size; ++i)
		{
			if (byteUsage[i] == ByteUsage::ImplicitConst) static_cast<char*>(obj)[i] = implicitValues[i];
		}
	}
}

void* TypeInfo::upcast(void* obj, const TypeName& name) const
{
	//If referring to self, nothing to do
	if (name == this->name) return obj;

	for (const ParentInfo& parent : parents)
	{
		void* objAsImmediateParent = ((char*)obj) + parent.offset;
		
		//Try matching parent, recursing
		void* out = parent.typeName.resolve()->upcast(objAsImmediateParent, name);
		if (out) return out;
	}
	
	//Not a parent
	return nullptr;
}

void TypeInfo::doLateBinding()
{
	//Deferred from captureCDO: Mark all fields as used
	assert(byteUsage);
	walkFields(
		[=](const FieldInfo& fi) {
			memset(byteUsage+fi.offset, (uint8_t)ByteUsage::ExplicitField, fi.size);
		},
		MemberVisibility::All,
		true
	);
}

void TypeInfo::create_internalFinalize()
{
	byteUsage = (ByteUsage*)malloc(size);
	memset(byteUsage, (uint8_t)ByteUsage::Unknown, size);
}
