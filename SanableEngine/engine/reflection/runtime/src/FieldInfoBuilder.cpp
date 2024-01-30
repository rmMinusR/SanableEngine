#include "FieldInfoBuilder.hpp"

FieldInfoBuilder::FieldInfoBuilder(const TypeName& declaredType, const std::string& name, size_t size, std::function<ptrdiff_t(const void*)> accessor, MemberVisibility visibility) :
	declaredType(declaredType),
	name(name),
	size(size),
	accessor(accessor),
	visibility(visibility)
{
}

FieldInfo FieldInfoBuilder::build(const TypeName& ownerName, const void* ownerImage) const
{
	FieldInfo f;

	f.owner = ownerName;
	f.type = declaredType;
	f.name = name;
	f.size = size;
	f.offset = accessor(ownerImage);
	f.visibility = visibility;

	return f;
}
