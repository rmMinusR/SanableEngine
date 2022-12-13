#pragma once

#include <vector>
#include "TypedMemoryPool.inl"

class GameObject;
class EngineCore;

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

		//Search for pool matching type (FIXME slow?)
		for (RawMemoryPool* p : pools) if (out = dynamic_cast<TypedMemoryPool<TObj>*>(p)) return out;
		
		//If set to create on fallback, do so
		if (!out && fallbackCreate) pools.push_back((RawMemoryPool*)(out = new TypedMemoryPool<TObj>(PoolSettings<TObj>::maxObjectCount)));

		return out;
	}

	std::vector<RawMemoryPool*> pools;

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
			for (RawMemoryPool* p : pools) if (p->contains(obj)) { pool = p; break; }
		}

		assert(pool);
		assert(pool->contains(obj));

		pool->release(obj);
	}

private:
	void init();
	void cleanup();
	friend class EngineCore;
};
