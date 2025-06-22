#include "TypeBuilder.hpp"

#include "ModuleTypeRegistry.hpp"

TypeBuilder::TypeBuilder()
{
}

void TypeBuilder::addParent_internal(const TypeName& parent, size_t parentSize, const std::function<void* (void*)>& upcastFn, MemberVisibility visibility, ParentInfo::Virtualness virtualness)
{
	pendingParents.emplace_back(type.name, parent, type.layout.size, parentSize, upcastFn, visibility, virtualness);
}

void TypeBuilder::addField_internal(const TypeName& declaredType, const std::string& name, size_t size, std::function<ptrdiff_t(const void*)> accessor, MemberVisibility visibility)
{
	pendingFields.emplace_back(declaredType, name, size, accessor, visibility);
}

void TypeBuilder::addMemberFunction(const stix::MemberFunction& func, const std::string& name, MemberVisibility visibility, bool isVirtual)
{
	type.capabilities.memberFuncs.push_back(TypeInfo::Capabilities::MemberFuncRecord{ func, name, visibility, isVirtual });
}

void TypeBuilder::addStaticFunction(const stix::StaticFunction& func, const std::string& name, MemberVisibility visibility)
{
	type.capabilities.staticFuncs.push_back(TypeInfo::Capabilities::StaticFuncRecord{ func, name, visibility });
}

void TypeBuilder::addConstructor(const stix::StaticFunction& thunk, MemberVisibility visibility)
{
	type.capabilities.constructors.push_back(TypeInfo::Capabilities::ConstructorRecord{ thunk, visibility });
}

void TypeBuilder::captureClassImage_v1_internal(std::function<void(void*)> ctor, std::function<void(void*)> dtor)
{
	assert(!type.layout.byteUsage.empty());
	assert(type.layout.implicitValues.empty());
	
	//Prepare memory
	char* cdo1 = (char*)malloc(3 * type.layout.size);
	char* cdo2 = cdo1 + type.layout.size;
	char* cdo3 = cdo2 + type.layout.size;
	memset(cdo1, 0x00, type.layout.size);
	memset(cdo2, 0x88, type.layout.size);
	memset(cdo3, 0xFF, type.layout.size);

	//Create CDOs
	ctor(cdo1);
	ctor(cdo2);
	ctor(cdo3);

	////// BEGIN MAIN MAGIC //////

	//A bitset would be more space-efficient, but that's fixed size and probably slower for our purposes
	type.layout.byteUsage.clear();
	type.layout.byteUsage.resize(type.layout.size, TypeInfo::Layout::ByteUsage::ImplicitConst);
	
	//Detect constants
	type.layout.implicitValues.resize(type.layout.size);
	memcpy(type.layout.implicitValues.data(), cdo1, type.layout.size); //First one has no point of comparison

	auto cmpAgainst = [&](char* cmp)
	{
		for (size_t byteIndex = 0; byteIndex < type.layout.size; ++byteIndex) //... byte by byte...
		{
			//If we don't match, it's padding
			if (type.layout.implicitValues[byteIndex] != cmp[byteIndex])
			{
				type.layout.byteUsage[byteIndex] = TypeInfo::Layout::ByteUsage::Padding;
			}
		}
	};
	cmpAgainst(cdo2);
	cmpAgainst(cdo3);

	//Normally we'd scan each field and mark them as used, but we defer until we're registered with GlobalTypeRegistry
	//Otherwise we wouldn't be able to walk parents in the same translation unit for explicit fields

	////// END MAIN MAGIC //////
	
	dtor(cdo1);
	dtor(cdo2);
	dtor(cdo3);
	free(cdo1);
}

void TypeBuilder::captureClassImage_v2_internal(const DetectedConstants& image)
{
	assert(!type.layout.byteUsage.empty());
	assert(type.layout.implicitValues.empty());

	//Assume padding as default
	type.layout.implicitValues.resize(type.layout.size, (uint8_t)TypeInfo::Layout::ByteUsage::Padding);

	for (int i = 0; i < type.layout.size; ++i)
	{
		if (image.usage[i])
		{
			type.layout.byteUsage[i] = TypeInfo::Layout::ByteUsage::ImplicitConst;
			type.layout.implicitValues[i] = image.bytes[i];
		}
	}
}

void TypeBuilder::registerType(ModuleTypeRegistry* registry)
{
	//Resolve pending parents
	for (ParentInfoBuilder& p : pendingParents) type.layout.parents.push_back(p.buildFromClassImage(type.layout.implicitValues.data()));
	pendingParents.clear();

	//Resolve pending fields
	for (const FieldInfoBuilder& p : pendingFields) type.layout.fields.push_back(p.build(type.name, type.layout.implicitValues.data()));
	pendingFields.clear();

	//DO NOT update byte usage: this is deferred to TypeInfo::doLateBinding
	
	type.hash = type.computeHash();

	assert(registry->lookupType(type.name) == nullptr);
	registry->types.push_back(type);
}
