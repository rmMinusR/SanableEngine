#include "TypeBuilder.hpp"

#include "ModuleTypeRegistry.hpp"

TypeBuilder::TypeBuilder()
{
}

void TypeBuilder::addParent_internal(const TypeName& parent, size_t size, const std::function<void* (void*)>& upcastFn)
{
	ParentInfo p;
	p.typeName = parent;
	p.size = size;
	p.visibility = MemberVisibility::Public; //TEMP TODO make RTTI generation step

	assert(size < 256); //TEMP measure. Can fix with multiple waves to detect
	size_t nPossibleAddrs = size / sizeof(void*) + 1;
	void* virtAddrDetector[nPossibleAddrs];
	memset(virtAddrDetector, 0, size);

	//Detect offset/virtualness
	void* parentAddr = upcastFn(virtAddrDetector);
	p.isVirtual = !( virtAddrDetector <= parentAddr && parentAddr < virtAddrDetector+nPossibleAddrs ); //TODO what about inheritance where the common base's virtuality varies?

	if (!p.isVirtual)
	{
		//Simple case: Object is embedded by value
		p.offset = size_t(((char*)parentAddr) - ((char*)virtAddrDetector));
	}
	else
	{
		//Complex case: A pointer-to-parent is embedded, scan until we find the right address
		bool found = false;
		for (size_t i = 0; i < nPossibleAddrs && !found; ++i)
		{
			//Select only one pointer at a time
			memset(virtAddrDetector, 0, size);
			virtAddrDetector[i] = (void*)0xDEADBEEF; //Arbitrary, but not nullptr

			if (upcastFn(virtAddrDetector))
			{
				//Cast was successful! Calculate relative address.
				p.offset = size_t(((char*)i) - ((char*)virtAddrDetector));
				found = true;
			}
		}

		assert(found);
	}
	
	//Done
	type.parents.push_back(p);
}

void TypeBuilder::addField_internal(const TypeName& declaredType, const std::string& name, size_t size, size_t offset)
{
	FieldInfo f;
	f.owner = type.name;
	f.type = declaredType;
	f.name = name;
	f.size = size;
	f.offset = offset;
	f.visibility = MemberVisibility::Public; //TEMP TODO make RTTI generation step
	type.fields.push_back(f);
}

void TypeBuilder::captureCDO_internal(const std::vector<void*>& instances)
{
	assert(type.byteUsage != nullptr);
	assert(type.implicitValues == nullptr);
	assert(instances.size() >= 2);
	
	//A bitset would be more space-efficient, but that's fixed size and probably slower for our purposes
	memset(type.byteUsage, (uint8_t)TypeInfo::ByteUsage::ImplicitConst, type.size);
	
	//Detect constants
	type.implicitValues = (char*) malloc(type.size);
	memcpy(type.implicitValues, instances[0], type.size); //First one has no point of comparison

	for (int i = 1; i < instances.size(); ++i) //Scan each additional provided instance...
	{
		char* cmp = (char*)instances[i];
		for (size_t byteIndex = 0; byteIndex < type.size; ++byteIndex) //... byte by byte...
		{
			//If we don't match, it's padding
			if (type.implicitValues[byteIndex] != cmp[byteIndex])
			{
				type.byteUsage[byteIndex] = TypeInfo::ByteUsage::Padding;
			}
		}
	}

	//Normally we'd scan each field and mark them as used, but we defer until we're registered with GlobalTypeRegistry
	//Otherwise we wouldn't be able to walk parents in the same translation unit
}

void TypeBuilder::registerType(ModuleTypeRegistry* registry)
{
	assert(registry->lookupType(type.name) == nullptr);
	registry->types.push_back(type);
}