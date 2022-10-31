#pragma once

#include <SDL_pixels.h>

#include "Component.hpp"

class RectangleCollider;
class RectangleRenderer;

class ColliderColorChanger :
	public Component,
	public IUpdatable
{
private:
	SDL_Color normalColor;
	SDL_Color overlapColor;

	RectangleCollider* collider;
	RectangleRenderer* renderer;

public:
	ColliderColorChanger(GameObject& owner, SDL_Color normalColor, SDL_Color overlapColor);
	~ColliderColorChanger();

	void Update() override;
};
