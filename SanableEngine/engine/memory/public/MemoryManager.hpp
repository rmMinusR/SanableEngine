#pragma once

#include <unordered_map>
#include <vector>
#include "TypedMemoryPool.inl"

class GameObject;
class Application;
struct TypeInfo;
class PluginManager;

template<>
struct PoolSettings<GameObject, 64> {};

class MemoryManager
{
private:
	std::vector<GenericTypedMemoryPool*> pools;
	//std::unordered_map<std::string, RawMemoryPool*> pools;

public:
	template<typename TObj>
	inline TypedMemoryPool<TObj>* getSpecificPool(bool fallbackCreate);

	template<typename TObj, typename... TCtorArgs>
	inline TObj* create(TCtorArgs... ctorArgs);

	template<typename TObj>
	void destroy(TObj* obj);

	template<typename TObj>
	void destroyPool();

private:
	ENGINEMEM_API void init();
	ENGINEMEM_API void cleanup();
	friend class Application;

	friend class PluginManager;

	ENGINEMEM_API void ensureFresh();
	ENGINEMEM_API void updatePointers(const MemoryMapper& remapper);
};



template<typename TObj>
inline TypedMemoryPool<TObj>* MemoryManager::getSpecificPool(bool fallbackCreate)
{
	TypedMemoryPool<TObj>* out = nullptr;

	//Search for pool matching typename
	auto it = std::find_if(pools.cbegin(), pools.cend(), [&](GenericTypedMemoryPool* p) { return p->contentsType.name == TypeName::create<TObj>(); });
	if (it != pools.cend())
	{
		out = (TypedMemoryPool<TObj>*)*it; //TODO sanity dynamic_cast?
	}
	//for (RawMemoryPool* p : pools) if (out = dynamic_cast<TypedMemoryPool<TObj>*>(p)) return out;

	//If set to create on fallback, do so
	if (!out && fallbackCreate)
	{
		pools.push_back( out = new TypedMemoryPool<TObj>(PoolSettings<TObj>::maxObjectCount) );
	}

	return out;
}

template<typename TObj, typename... TCtorArgs>
inline TObj* MemoryManager::create(TCtorArgs... ctorArgs)
{
	return getSpecificPool<TObj>(true)->emplace(ctorArgs...);
}

template<typename TObj>
void MemoryManager::destroy(TObj* obj)
{
	//Try direct lookup first
	RawMemoryPool* pool = (RawMemoryPool*) getSpecificPool<TObj>(false);

	//If that fails, search every pool to find owner
	if (!pool)
	{
		for (GenericTypedMemoryPool* i : pools) if (i->contains(obj)) { pool = i; break; }
	}

	assert(pool);
	assert(pool->contains(obj));

	pool->release(obj);
}

template<typename TObj>
void MemoryManager::destroyPool()
{
	auto it = std::find_if(pools.cbegin(), pools.cend(), [&](GenericTypedMemoryPool* p) { return p->contentsType.name == TypeName::create<TObj>(); });
	if (it != pools.cend())
	{
		delete *it;
		pools.erase(it);
	}
}
