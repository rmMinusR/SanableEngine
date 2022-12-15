#include "ColliderColorChanger.hpp"

#include "GameObject.hpp"
#include "RectangleCollider.hpp"
#include "RectangleRenderer.hpp"
#include "EngineCore.hpp"

ColliderColorChanger::ColliderColorChanger(GameObject* owner, SDL_Color normalColor, SDL_Color overlapColor) :
	Component(owner),
	normalColor(normalColor),
	overlapColor(overlapColor),
	collider(owner->GetComponent<RectangleCollider>()),
	renderer(owner->GetComponent<RectangleRenderer>())
{
	assert(collider);
	assert(renderer);
}

ColliderColorChanger::~ColliderColorChanger()
{
}

void ColliderColorChanger::Update()
{
	int nHits = collider->GetCollisions(nullptr);
	RectangleCollider** hits = engine.getFrameAllocator()->alloc<RectangleCollider*>(nHits);

	renderer->SetColor(nHits>0 ? overlapColor : normalColor);

	for (int i = 0; i < nHits; ++i) printf("0x%p is colliding with 0x%p\n", this, hits[i]);
}
