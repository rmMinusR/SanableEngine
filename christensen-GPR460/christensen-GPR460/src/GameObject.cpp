#include "GameObject.hpp"

#include <utility>
#include <cassert>

GameObject::GameObject() : GameObject(Transform(0, 0, 0))
{
}

GameObject::GameObject(Transform&& initialTransform) :
	transform(std::move(initialTransform)),
	renderer(nullptr),
	collider(nullptr),
	player(nullptr),
	colorChanger(nullptr)
{
}

GameObject::~GameObject()
{
	if (renderer)
	{
		delete renderer;
		renderer = nullptr;
	}

	if (collider)
	{
		delete collider;
		collider = nullptr;
	}

	if (player)
	{
		delete player;
		player = nullptr;
	}

	if (colorChanger)
	{
		delete colorChanger;
		colorChanger = nullptr;
	}
}

RectangleRenderer* GameObject::CreateRenderer(float w, float h)
{
	assert(renderer == nullptr);
	renderer = new RectangleRenderer(w, h);
	return renderer;
}

RectangleCollider* GameObject::CreateCollider(float w, float h)
{
	assert(collider == nullptr);
	collider = new RectangleCollider(w, h);
	return collider;
}
