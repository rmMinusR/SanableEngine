#include "GameObject.hpp"

#include <utility>
#include <cassert>

#include "EngineCore.hpp"
#include "Component.hpp"
#include "Hotswap.inl"

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

GameObject::GameObject(EngineCore* engine) :
	transform(0, 0, 0),
	engine(engine)
{
}

GameObject::~GameObject()
{
	if (components.size() != 0)
	{
		for (Component* c : components) engine->getMemoryManager()->destroy(c);
		components.clear();
	}
}
