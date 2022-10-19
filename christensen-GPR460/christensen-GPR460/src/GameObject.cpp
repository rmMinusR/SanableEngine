#include "GameObject.hpp"

#include <utility>
#include <cassert>

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
	renderer = DBG_NEW RectangleRenderer(*this, w, h, color);
	return renderer;
}

RectangleCollider* GameObject::CreateCollider(float w, float h)
{
	assert(collider == nullptr);
	collider = DBG_NEW RectangleCollider(*this, w, h);
	return collider;
}

PlayerController* GameObject::CreatePlayerController()
{
	assert(player == nullptr);
	player = DBG_NEW PlayerController(*this);
	return player;
}

ColliderColorChanger* GameObject::CreateColliderColorChanger(SDL_Color normalColor, SDL_Color overlapColor)
{
	assert(colorChanger == nullptr);
	colorChanger = DBG_NEW ColliderColorChanger(*this, normalColor, overlapColor);
	return colorChanger;
}

void GameObject::Update()
{
	if (player) player->Update();
	if (colorChanger) colorChanger->Update();
}

void GameObject::Render()
{
	if (renderer) renderer->Render();
}
