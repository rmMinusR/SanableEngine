#pragma once

#include <unordered_map>
#include <vector>
#include "TypedMemoryPool.inl"

class GameObject;
class EngineCore;
struct StableTypeInfo;
class PluginManager;

template<>
struct PoolSettings<GameObject, 64> {};

class MemoryManager
{
private:
	struct PoolRecord
	{
		StableTypeInfo poolType;
		RawMemoryPool* pool;

		template<typename TObj>
		static PoolRecord create(TypedMemoryPool<TObj>* pool) {
			PoolRecord r;
			r.pool = (RawMemoryPool*)pool;
			r.poolType = StableTypeInfo::blank<TypedMemoryPool<TObj>>();
			return std::move(r);
		}
	};

	std::vector<PoolRecord> pools;
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
	void init();
	void cleanup();
	friend class EngineCore;

	void refreshObjects(std::vector<StableTypeInfo const*> refreshers);
	friend class PluginManager;
};



template<typename TObj>
inline TypedMemoryPool<TObj>* MemoryManager::getSpecificPool(bool fallbackCreate)
{
	TypedMemoryPool<TObj>* out = nullptr;

	//Search for pool matching typename
	auto it = std::find_if(pools.cbegin(), pools.cend(), [&](const PoolRecord& r) { return r.poolType.name == TypeName::create<TypedMemoryPool<TObj>>(); });
	if (it != pools.cend())
	{
		out = (TypedMemoryPool<TObj>*)it->pool;
		//assert(dynamic_cast<TypedMemoryPool<TObj>*>(it->second) != nullptr);
	}
	//for (RawMemoryPool* p : pools) if (out = dynamic_cast<TypedMemoryPool<TObj>*>(p)) return out;

	//If set to create on fallback, do so
	if (!out && fallbackCreate)
	{
		pools.push_back(PoolRecord::create<TObj>( out = new TypedMemoryPool<TObj>(PoolSettings<TObj>::maxObjectCount) ));
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
		for (auto& i : pools) if (i.pool->contains(obj)) { pool = i.pool; break; }
	}

	assert(pool);
	assert(pool->contains(obj));

	pool->release(obj);
}

template<typename TObj>
void MemoryManager::destroyPool()
{
	auto it = std::find_if(pools.cbegin(), pools.cend(), [&](const PoolRecord& r) { return r.poolType.name == TypeName::create<TypedMemoryPool<TObj>>(); });
	if (it != pools.cend())
	{
		delete it->pool;
		pools.erase(it);
	}
}
