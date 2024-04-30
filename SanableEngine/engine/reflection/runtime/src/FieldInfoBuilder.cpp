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
	return FieldInfo(size, accessor(ownerImage), ownerName, declaredType, name, visibility);;
}
