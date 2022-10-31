#include "ColliderColorChanger.hpp"

#include "GameObject.hpp"
#include "RectangleCollider.hpp"
#include "RectangleRenderer.hpp"

ColliderColorChanger::ColliderColorChanger(GameObject& owner, SDL_Color normalColor, SDL_Color overlapColor) :
	Component(owner),
	normalColor(normalColor),
	overlapColor(overlapColor),
	collider(owner.GetComponent<RectangleCollider>()),
	renderer(owner.GetComponent<RectangleRenderer>())
{
	assert(collider);
	assert(renderer);
}

ColliderColorChanger::~ColliderColorChanger()
{
}

void ColliderColorChanger::Update()
{
	renderer->SetColor(collider->CheckCollisionAny() ? overlapColor : normalColor);
}
