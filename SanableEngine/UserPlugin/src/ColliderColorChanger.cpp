#include "ColliderColorChanger.hpp"

#include "GameObject.hpp"
#include "RectangleCollider.hpp"
#include "RectangleRenderer.hpp"

void ColliderColorChanger::BindToGameObject(GameObject* obj)
{
	Component::BindToGameObject(obj);

	collider = obj->GetComponent<RectangleCollider>();
	renderer = obj->GetComponent<RectangleRenderer>();

	assert(collider);
	assert(renderer);
}

ColliderColorChanger::ColliderColorChanger(SDL_Color normalColor, SDL_Color overlapColor) :
	Component(),
	normalColor(normalColor),
	overlapColor(overlapColor),
	collider(nullptr),
	renderer(nullptr)
{
}

ColliderColorChanger::~ColliderColorChanger()
{
}

void ColliderColorChanger::Update()
{
	renderer->SetColor(collider->CheckCollisionAny() ? overlapColor : normalColor);
}
