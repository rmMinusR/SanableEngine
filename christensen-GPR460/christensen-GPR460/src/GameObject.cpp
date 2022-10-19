#include "GameObject.hpp"

#include <utility>
#include <cassert>

#include <SDL_render.h>

#include "EngineCore.hpp"
#include "RectangleCollider.hpp"
#include "RectangleRenderer.hpp"

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

RectangleRenderer* GameObject::CreateRenderer(float w, float h, SDL_Color color)
{
	assert(renderer == nullptr);
	renderer = new RectangleRenderer(*this, w, h, color);
	return renderer;
}

RectangleCollider* GameObject::CreateCollider(float w, float h)
{
	assert(collider == nullptr);
	collider = new RectangleCollider(*this, w, h);
	return collider;
}
