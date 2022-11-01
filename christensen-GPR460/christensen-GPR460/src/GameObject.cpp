#include "GameObject.hpp"

#include <utility>

#include <SDL_render.h>

#include "EngineCore.hpp"
#include "RectangleCollider.hpp"
#include "RectangleRenderer.hpp"
#include "PlayerController.hpp"
#include "ColliderColorChanger.hpp"

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
