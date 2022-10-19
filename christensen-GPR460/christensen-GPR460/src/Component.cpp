#include "Component.hpp"

#include "GameObject.hpp"

Component::Component(GameObject& owner) :
	gameObject(&owner)
{
}

Component::~Component()
{
	//TODO ensure gameObject knows to remove this
}
