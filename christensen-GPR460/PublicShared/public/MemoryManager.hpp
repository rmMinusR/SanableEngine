#pragma once

#include <unordered_map>
#include <vector>
#include "TypedMemoryPool.inl"
#include "Hotswap.hpp"

class GameObject;
class EngineCore;
struct HotswapTypeData;
class PluginManager;

template<>
struct PoolSettings<GameObject, 64> {};

class MemoryManager
{
private:
	struct PoolRecord
	{
		std::string objectType;
		std::string poolType;
		RawMemoryPool* pool;

		template<typename TObj>
		static PoolRecord create(TypedMemoryPool<TObj>* pool) {
			PoolRecord r;
			r.pool = (RawMemoryPool*)pool;
			r.objectType = HotswapTypeData::extractName<TObj>();
			r.poolType = HotswapTypeData::extractName<TypedMemoryPool<TObj>>();
			return std::move(r);
		}
	};

	std::vector<PoolRecord> pools;
	//std::unordered_map<std::string, RawMemoryPool*> pools;

public:
	template<typename TObj>
	TypedMemoryPool<TObj>* getSpecificPool(bool fallbackCreate);

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

	void refreshVtables(std::vector<HotswapTypeData*> refreshers);
	friend class PluginManager;
};



template<typename TObj>
inline TypedMemoryPool<TObj>* MemoryManager::getSpecificPool(bool fallbackCreate)
{
	//Search for pool matching typename
	std::string tName = HotswapTypeData::extractName<TObj>();
	for (const PoolRecord& r : pools) if (tName == r.objectType) return (TypedMemoryPool<TObj>*)r.pool;

	//If set to create on fallback, do so
	if (fallbackCreate)
	{
		TypedMemoryPool<TObj>* out = out = new TypedMemoryPool<TObj>;
		pools.push_back(PoolRecord::create<TObj>(out));
		return out;
	}
	else return nullptr;
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
	std::string tName = HotswapTypeData::extractName<TObj>();
	auto it = std::find_if(pools.cbegin(), pools.cend(), [&](const PoolRecord& r) { return tName == r.objectType; });
	if (it != pools.cend())
	{
		delete it->pool;
		pools.erase(it);
	}
}
