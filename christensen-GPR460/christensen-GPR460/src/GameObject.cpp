#include "GameObject.hpp"

#include <utility>

#include <SDL_render.h>

#include "EngineCore.hpp"
#include "RectangleCollider.hpp"
#include "RectangleRenderer.hpp"
#include "PlayerController.hpp"
#include "ColliderColorChanger.hpp"

GameObject::GameObject() : GameObject(Transform(0, 0, 0))
{
}

GameObject::GameObject(Transform&& initialTransform) :
	transform(std::move(initialTransform))
{
}

GameObject::~GameObject()
{
	for (Component* c : components) delete c;
	components.clear();
}
