#pragma once

#include <unordered_map>
#include <vector>
#include "TypedMemoryPool.inl"
#include "Hotswap.hpp"

class GameObject;
class EngineCore;
struct HotswapTypeData;
class PluginManager;

template<typename T, size_t _maxObjectCount = 32>
struct PoolSettings
{
public:
	constexpr static size_t maxObjectCount = _maxObjectCount;
};

template<>
struct PoolSettings<GameObject, 64> {};

class MemoryManager
{
private:
	template<typename TObj>
	TypedMemoryPool<TObj>* getPool(bool fallbackCreate)
	{
		TypedMemoryPool<TObj>* out = nullptr;

		//Search for pool matching typename
		auto it = pools.find(HotswapTypeData::extractName<TObj>());
		if (it != pools.cend())
		{
			out = (TypedMemoryPool<TObj>*)it->second;
			//assert(dynamic_cast<TypedMemoryPool<TObj>*>(it->second) != nullptr);
		}
		//for (RawMemoryPool* p : pools) if (out = dynamic_cast<TypedMemoryPool<TObj>*>(p)) return out;
		
		//If set to create on fallback, do so
		if (!out && fallbackCreate)
		{
			pools.emplace(HotswapTypeData::extractName<TObj>(), (RawMemoryPool*)(out = new TypedMemoryPool<TObj>(PoolSettings<TObj>::maxObjectCount)));
		}

		return out;
	}

	//std::vector<RawMemoryPool*> pools;
	std::unordered_map<std::string, RawMemoryPool*> pools;

public:
	template<typename TObj, typename... TCtorArgs>
	inline TObj* create(TCtorArgs... ctorArgs)
	{
		return getPool<TObj>(true)->emplace(ctorArgs...);
	}

	template<typename TObj>
	inline void destroy(TObj* obj)
	{
		//Try direct lookup first
		RawMemoryPool* pool = (RawMemoryPool*) getPool<TObj>(false);

		//If that fails, search every pool to find owner
		if (!pool)
		{
			for (auto& i : pools) if (i.second->contains(obj)) { pool = i.second; break; }
		}

		assert(pool);
		assert(pool->contains(obj));

		pool->release(obj);
	}

private:
	void init();
	void cleanup();
	friend class EngineCore;

	void refreshVtables(std::vector<HotswapTypeData*> refreshers);
	friend class PluginManager;
};
