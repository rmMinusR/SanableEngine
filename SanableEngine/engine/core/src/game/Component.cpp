#include "game/Component.hpp"

#include <cassert>

#include "application/Application.hpp"
#include "game/GameObject.hpp"
#include "Material.hpp"

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

IUpdatable::~IUpdatable()
{
}

const ShaderProgram* I3DRenderable::getShader() const
{
	const Material* material = getMaterial();
	return material ? material->getShader() : nullptr;
}

I3DRenderable::~I3DRenderable()
{
}
