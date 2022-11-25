#include "GameObject.hpp"

#include <cassert>
#include <utility>

#include <SDL_render.h>

#include <SerializationRegistryEntry.hpp>
#include <SerializedObject.hpp>

#include "EngineCore.hpp"
#include "RectangleCollider.hpp"
#include "RectangleRenderer.hpp"
#include "PlayerController.hpp"
#include "ColliderColorChanger.hpp"

object_id_t GameObject::genID()
{
	object_id_t id;
	do {
		id = (object_id_t)rand();
	} while (!engine.getGameObject(id));

	return id;
}

GameObject::GameObject() : GameObject(genID())
{
}

GameObject::GameObject(object_id_t id) :
	transform(0, 0, 0),
	id(id)
{
	assert(!engine.getGameObject(id));
}

GameObject::~GameObject()
{
	//for (Component* c : components) delete c;
	//for (Component* c : components) MemoryManager::destroy(c);
	//FIXME no way to cleanly free! (unless we introduce wrapper objects...)
	components.clear();
}

const SerializationRegistryEntry GameObject::SERIALIZATION_REGISTRY_ENTRY = AUTO_SerializationRegistryEntry(GameObject, engine.addGameObject(), /* nothing to set */, engine.destroy(val));

SerializationRegistryEntry const* GameObject::getRegistryEntry() const
{
	return &SERIALIZATION_REGISTRY_ENTRY;
}

void GameObject::binarySerializeMembers(std::ostream& out) const
{
	SerializedObject transformSerializer;
	if (transformSerializer.serialize(&transform)) transformSerializer.write(out);

	size_t componentCount = components.size();
	char componentCountBytes[componentCountHeaderSize];
	memset(componentCountBytes, 0, componentCountHeaderSize);
	memcpy(componentCountBytes, &componentCount, min(sizeof(size_t), componentCountHeaderSize));
	out.write(componentCountBytes, componentCountHeaderSize);

	for (Component* c : components)
	{
		SerializedObject serializer;
		if (serializer.serialize(c)) serializer.write(out);
	}
}

void GameObject::binaryDeserializeMembers(std::istream& in)
{
	clear();

	while (!in.eof())
	{
		SerializedObject deserializer;
		if (deserializer.parse(in)) deserializer.deserializeAndInject();
	}
}
