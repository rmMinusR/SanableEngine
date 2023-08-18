#pragma once

#include <SDL_pixels.h>

#include "StandardReflectionHooks.hpp"

#include "Component.hpp"

class RectangleCollider;
class RectangleRenderer;

class ColliderColorChanger :
	public Component,
	public IUpdatable
{
	SANABLE_REFLECTION_HOOKS

private:
	SDL_Color normalColor;
	SDL_Color overlapColor;

	RectangleCollider* collider;
	RectangleRenderer* renderer;

	ColliderColorChanger() = default;
protected:
	void BindToGameObject(GameObject* obj) override;

public:
	ColliderColorChanger(SDL_Color normalColor, SDL_Color overlapColor);
	~ColliderColorChanger();

	void Update() override;
};
