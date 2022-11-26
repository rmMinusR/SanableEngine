#include "Component.hpp"

#include <cassert>

#include "GameObject.hpp"
#include "EngineCore.hpp"

Component::Component(GameObject* owner) :
	gameObject(nullptr),
	gameObjectID(owner ? owner->getID() : 0)
{
}

Component::~Component()
{
}

void Component::bindGameObject()
{
	assert(gameObject == nullptr);

	gameObject = engine.getGameObject(gameObjectID);
	gameObject->AddOwnedComponent(this);
}

void Component::binarySerializeMembers(std::ostream& out) const
{
	binWriteRaw(gameObjectID, out);
}

void Component::binaryDeserializeMembers(std::istream& in)
{
	binReadRaw(gameObjectID, in);
}

IUpdatable::IUpdatable()
{
	engine.getUpdatables()->add(this);
}

IUpdatable::~IUpdatable()
{
	engine.getUpdatables()->remove(this);
}

IRenderable::IRenderable()
{
	engine.getRenderables()->add(this);
}

IRenderable::~IRenderable()
{
	engine.getRenderables()->remove(this);
}
