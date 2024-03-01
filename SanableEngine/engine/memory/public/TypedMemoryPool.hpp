#pragma once

#include "MemoryPoolCommon.hpp"
#include "RawMemoryPool.hpp"
#include "TypeInfo.hpp"

class MemoryManager;
class GenericTypedMemoryPool;

//Strongly typed pointers (recommended)
//Just a compile-time typesafe view onto GenericTypedMemoryPool, that's also reload-safe
template<typename TObj>
class TypedMemoryPool
{
private:
	friend class MemoryManager;
	GenericTypedMemoryPool* impl;
public:
	TypedMemoryPool(GenericTypedMemoryPool* impl) : impl(impl) {}

	//Allocates memory and creates an object.
	template<typename... TCtorArgs>
	TObj* emplace(TCtorArgs... ctorArgs)
	{
		//Allocate memory
		TObj* pObj = (TObj*) impl->allocate();
#ifndef TEST_MEMORY
		assert(pObj);
#endif

		//Construct object
		if (pObj) new (pObj) TObj(ctorArgs...);

		return pObj;
	}

	//Pass through
	inline void release(TObj* obj) { impl->release(obj); }

	inline RawMemoryPool::const_iterator cbegin() const { return impl->cbegin(); }
	inline RawMemoryPool::const_iterator cend  () const { return impl->cend  (); }
	
#ifdef TEST_MEMORY
	//INTERNAL TESTING USE ONLY
	inline GenericTypedMemoryPool* asGeneric() { return this; }
#endif

protected:
	TypedMemoryPool(TypedMemoryPool&&) = delete;
	TypedMemoryPool(const TypedMemoryPool&) = delete;
};


//Backend for TypedMemoryPool so we can still safely access common data
class GenericTypedMemoryPool : public RawMemoryPool
{
protected:
	TypeInfo contentsType;
	TypedMemoryPool<void> view; //Needs to be cast to be used safely

	ENGINEMEM_API GenericTypedMemoryPool(size_t maxNumObjects, const TypeInfo& contentsType);
public:
	ENGINEMEM_API ~GenericTypedMemoryPool();

	ENGINEMEM_API bool isLoaded() const;

	template<typename TObj>
	[[nodiscard]] static GenericTypedMemoryPool* create(size_t maxNumObjects = 64)
	{
		return new GenericTypedMemoryPool(
			maxNumObjects,
			TypeInfo::createDummy<TObj>() //No need to resolve dummy TypeInfo here. Engine will call refreshObjects after all TypeInfos are registered.
		);
	}

	//INTERNAL USE ONLY
	ENGINEMEM_API void refreshObjects(const TypeInfo& newTypeData, MemoryMapper* remapper);
};
