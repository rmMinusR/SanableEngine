#include "Component.hpp"

#include <cassert>

#include "GameObject.hpp"
#include "EngineCore.hpp"

Component::Component(GameObject* owner) :
	gameObject(owner),
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
