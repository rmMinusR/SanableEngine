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
	this->align   = cpy.align;
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
	this->align   = std::move(mov.align);
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
				if (parentType)
				{
					//Can't walk what isn't loaded
					parentType->walkFields(
						visitor,
						visibilityFlags,
						true
					);
				}
				else
				{
					printf("ERROR: %s (parent of %s) was not loaded. Cannot walk all fields.\n", parent.typeName.c_str(), name.c_str());
				}
			}
		}
	}

	//Walk own fields
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

	//Try inherited virtuals first
	for (const ParentInfo& parent : parents)
	{
		if (parent.virtualness == ParentInfo::Virtualness::VirtualInherited)
		{
			void* objAsImmediateParent = ((char*)obj) + parent.offset;
		
			//Try matching parent, recursing
			void* out = parent.typeName.resolve()->upcast(objAsImmediateParent, name);
			if (out) return out;
		}
	}

	//Then try normal inherited
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

bool TypeInfo::matchesExact(void* obj) const
{
	assert(byteUsage);
	assert(implicitValues);

	for (size_t i = 0; i < size; ++i)
	{
		if (byteUsage[i] == ByteUsage::ImplicitConst && reinterpret_cast<char*>(obj)[i] != implicitValues[i]) return false; //If implicit const detected, value must match
	}

	return true;
}

void TypeInfo::doLateBinding()
{
	//Deferred from captureCDO: Mark all fields as used
	assert(byteUsage);
	walkFields(
		[&](const FieldInfo& fi) {
			ptrdiff_t root = fi.offset + (ptrdiff_t)this->upcast(nullptr, fi.owner);
			assert(byteUsage[root] != ByteUsage::ImplicitConst && "Attempted to overwrite data (usage clobbering)");
			memset(byteUsage+root, (uint8_t)ByteUsage::ExplicitField, fi.size);
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
