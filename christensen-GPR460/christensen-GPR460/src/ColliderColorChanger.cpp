#include "ColliderColorChanger.hpp"

#include "GameObject.hpp"
#include "RectangleCollider.hpp"
#include "RectangleRenderer.hpp"

ColliderColorChanger::ColliderColorChanger(GameObject& owner, SDL_Color normalColor, SDL_Color overlapColor) :
	Component(owner),
	normalColor(normalColor),
	overlapColor(overlapColor)
{
}

ColliderColorChanger::~ColliderColorChanger()
{
}

void ColliderColorChanger::Update()
{
	gameObject->GetRenderer()->SetColor(gameObject->GetCollider()->CheckCollisionAny() ? overlapColor : normalColor);
}
