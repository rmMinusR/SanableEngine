#include "GameObject.hpp"

#include <utility>

#include <SDL_render.h>

#include "EngineCore.hpp"
#include "RectangleCollider.hpp"
#include "RectangleRenderer.hpp"
#include "PlayerController.hpp"
#include "ColliderColorChanger.hpp"

void GameObject::BindComponent(Component* c)
{
	assert(c->gameObject == nullptr || c->gameObject == this);
	c->BindToGameObject(this);

	IUpdatable* u = dynamic_cast<IUpdatable*>(c);
	if (u) engine.updateList.add(u);

	IRenderable* r = dynamic_cast<IRenderable*>(c);
	if (r) engine.renderList.add(r);
}

void GameObject::InvokeStart()
{
	for (Component* c : components) c->onStart();
}

GameObject::GameObject() :
	transform(0, 0, 0)
{
}

GameObject::~GameObject()
{
	//for (Component* c : components) delete c;
	//for (Component* c : components) MemoryManager::destroy(c);
	//FIXME no way to cleanly free! (unless we introduce wrapper objects...)
	components.clear();
}
