#include "Component.hpp"

#include <cassert>

#include "GameObject.hpp"
#include "EngineCore.hpp"

Component::Component(GameObject* owner) :
	gameObject(owner)
{
	assert(owner);
}

Component::~Component()
{
	//TODO ensure gameObject knows to remove this
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
