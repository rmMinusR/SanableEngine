#include "Component.hpp"

#include <cassert>

#include "GameObject.hpp"
#include "EngineCore.hpp"

Component::Component(EngineCore* engine) :
	Component(engine, nullptr)
{
}

Component::Component(EngineCore* engine, GameObject* owner) :
	engine(engine),
	gameObject(nullptr),
	gameObjectID(owner ? owner->getID() : 0)
{
}

Component::~Component()
{
	//TODO ensure gameObject knows to remove this
}

void Component::BindToGameObject(GameObject* obj)
{
	assert(gameObject == nullptr);
	gameObject = obj;
}

void Component::binarySerializeMembers(std::ostream& out) const
{
	binWriteRaw(gameObjectID, out);
}

void Component::binaryDeserializeMembers(std::istream& in)
{
	binReadRaw(gameObjectID, in);

	gameObject = engine->getGameObject(gameObjectID);
	gameObject->AddOwnedComponent(this);
}

void Component::onStart()
{
}
