#include "Component.hpp"

#include <cassert>

#include "GameObject.hpp"
#include "Application.hpp"

void Component::BindToGameObject(GameObject* obj)
{
	assert(gameObject == nullptr);
	gameObject = obj;
}

Component::Component() :
	gameObject(nullptr)
{
}

Component::~Component()
{
	//TODO ensure gameObject knows to remove this
}

void Component::onStart()
{
}
