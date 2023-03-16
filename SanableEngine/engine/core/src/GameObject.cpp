#include "GameObject.hpp"

#include <cassert>
#include <utility>
#include <cassert>

#include <SerializationRegistryEntry.hpp>
#include <SerializedObject.hpp>

#include "EngineCore.hpp"
#include "Component.hpp"
#include "Hotswap.inl"

void GameObject::AddOwnedComponent(Component* c)
{
	assert(c->getGameObject() == this);
	assert(std::find(components.begin(), components.end(), c) == components.end());
	components.push_back(c);
}

void GameObject::BindComponent(Component* c)
{
	assert(c->gameObject == nullptr || c->gameObject == this);
	assert(std::find(components.cbegin(), components.cend(), c) == components.cend());

	components.push_back(c);
	c->BindToGameObject(this);

	IUpdatable* u = dynamic_cast<IUpdatable*>(c);
	if (u) engine->updateList.add(u);

	IRenderable* r = dynamic_cast<IRenderable*>(c);
	if (r) engine->renderList.add(r);

	c->onStart();
}

void GameObject::InvokeStart()
{
	for (Component* c : components) c->onStart();
}

object_id_t GameObject::genID(EngineCore* engine)
{
	object_id_t id;
	do {
		id = (object_id_t)rand();
	} while (engine->getGameObject(id));

	return id;
}

GameObject::GameObject(EngineCore* engine)
	: GameObject(engine, genID(engine))
{
}

GameObject::GameObject(EngineCore* engine, object_id_t id) :
	transform(),
	id(id),
	engine(engine)
{
	assert(!engine->getGameObject(id));
	transform.ownerID = id;
}

GameObject::~GameObject()
{
	if (components.size() != 0)
	{
		for (Component* c : components) engine->getMemoryManager()->destroy(c);
		components.clear();
	}
}

const SerializationRegistryEntry GameObject::SERIALIZATION_REGISTRY_ENTRY = AUTO_SerializationRegistryEntry(GameObject, engine->addGameObject()->binaryDeserializeMembers(in));

SerializationRegistryEntry const* GameObject::getRegistryEntry() const
{
	return &SERIALIZATION_REGISTRY_ENTRY;
}

void GameObject::binarySerializeMembers(std::ostream& out) const
{
	binWriteRaw(id, out);

	SerializedObject transformSerializer;
	if (!transformSerializer.serialize(&transform, out)) assert(false);

	for (Component* c : components)
	{
		SerializedObject serializer;
		if (!serializer.serialize(c, out)) assert(false);
	}
}

void GameObject::binaryDeserializeMembers(std::istream& in)
{
	for (Component* c : components) engine->getMemoryManager()->destroy(c);
	components.clear();

	object_id_t tmp_id;
	binReadRaw(tmp_id, in);
	assert(!engine->getGameObject(tmp_id)); //Make sure we don't end up with duplicate objects with same ID
	id = tmp_id;

	SerializedObject transformSerializer;
	if (!transformSerializer.parse(engine, in)) assert(false);

	while (!in.eof())
	{
		SerializedObject deserializer;
		deserializer.parse(engine, in);
	}
}
