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
	} while (engine.getGameObject(id));

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
	for (Component* c : components) MemoryManager::destroy_wide(c);
	components.clear();
}

const SerializationRegistryEntry GameObject::SERIALIZATION_REGISTRY_ENTRY = AUTO_SerializationRegistryEntry(GameObject, engine.addGameObject()->binaryDeserializeMembers(in));

void GameObject::RemoveComponent(Component* c)
{
	auto it = std::find(components.begin(), components.end(), c);
	MemoryManager::destroy_wide(*it);
	components.erase(it);
}

SerializationRegistryEntry const* GameObject::getRegistryEntry() const
{
	return &SERIALIZATION_REGISTRY_ENTRY;
}

void GameObject::binarySerializeMembers(std::ostream& out) const
{
	SerializedObject transformSerializer;
	if (!transformSerializer.serialize(&transform, out)) assert(false);

	binWriteRaw(id, out);

	for (Component* c : components)
	{
		SerializedObject serializer;
		if (!serializer.serialize(c, out)) assert(false);
	}
}

void GameObject::binaryDeserializeMembers(std::istream& in)
{
	for (Component* c : components) MemoryManager::destroy_narrow(c);
	components.clear();

	object_id_t tmp_id;
	binReadRaw(tmp_id, in);
	assert(!engine.getGameObject(tmp_id)); //Make sure we don't end up with duplicate objects with same ID
	id = tmp_id;

	while (!in.eof())
	{
		SerializedObject deserializer;
		if (!deserializer.parse(in)) assert(false);
	}
}
