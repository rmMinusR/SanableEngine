#pragma once

#include <unordered_map>
#include <vector>
#include <functional>
#include "GlobalTypeRegistry.hpp"
#include "TypedMemoryPool.hpp"

class GameObject;
class Application;
struct TypeInfo;
class PluginManager;
class _PoolCallBatcherBase;

class MemoryManager
{
private:
	std::vector<GenericTypedMemoryPool*> pools;
	//std::unordered_map<std::string, RawMemoryPool*> pools;
	ENGINEMEM_API void registerPool(GenericTypedMemoryPool* pool);

	uint64_t poolStateHash;
	GlobalTypeRegistry::Snapshot lastKnownRtti;

public:
	ENGINEMEM_API MemoryManager();
	ENGINEMEM_API ~MemoryManager();

	ENGINEMEM_API GenericTypedMemoryPool* getSpecificPool(const TypeName& type);
	template<typename TObj>
	inline TypedMemoryPool<TObj>* getSpecificPool(bool fallbackCreate);

	template<typename TObj, typename... TCtorArgs>
	inline TObj* create(TCtorArgs... ctorArgs);

	template<typename TObj>
	void destroy(TObj* obj);

	ENGINEMEM_API void foreachPool(const std::function<void(GenericTypedMemoryPool*)>& visitor);
	ENGINEMEM_API void foreachPool(const std::function<void(const GenericTypedMemoryPool*)>& visitor) const;

	ENGINEMEM_API void destroyPool(const TypeName& type);
	template<typename TObj>
	inline void destroyPool() { destroyPool(TypeName::create<TObj>()); }

	ENGINEMEM_API void ensureFresh(); //USE WITH CAUTION.

private:
	friend class Application;
	friend class PluginManager;

	ENGINEMEM_API void updatePointers(const MemoryMapper& remapper);

	friend class _PoolCallBatcherBase;
	ENGINEMEM_API uint64_t getPoolStateHash() const;
};

template<typename TObj>
inline TypedMemoryPool<TObj>* MemoryManager::getSpecificPool(bool fallbackCreate)
{
	GenericTypedMemoryPool* out = getSpecificPool(TypeName::create<TObj>());

	//If set to create on fallback, do so
	if (!out && fallbackCreate)
	{
		out = GenericTypedMemoryPool::create<TObj>();
		registerPool(out);
		return out->getView<TObj>();
	}

	return out ? out->getView<TObj>() : nullptr;
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
	GenericTypedMemoryPool* pool = getSpecificPool(TypeName::create<TObj>());

	//If that fails, search every pool to find owner
	if (!pool)
	{
		for (GenericTypedMemoryPool* i : pools) if (i->contains(obj)) { pool = i; break; }
	}

	if (pool && pool->contains(obj))
	{
		pool->release(obj);
	}
	else wprintf(L"WARNING: Cannot destroy %s at %p: pool does not exist", TypeName::create<TObj>().c_str(), obj);
}
