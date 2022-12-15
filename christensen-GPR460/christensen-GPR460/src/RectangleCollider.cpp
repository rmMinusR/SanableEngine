#include "RectangleCollider.hpp"

#include "Vector3.inl"
#include "GameObject.hpp"
#include "EngineCore.hpp"

std::vector<RectangleCollider*> RectangleCollider::REGISTRY;

RectangleCollider::RectangleCollider(GameObject* owner, float w, float h) :
	Component(owner),
	w(w),
	h(h)
{
	RectangleCollider::REGISTRY.push_back(this);
}

RectangleCollider::~RectangleCollider()
{
	RectangleCollider::REGISTRY.erase(std::find(RectangleCollider::REGISTRY.cbegin(), RectangleCollider::REGISTRY.cend(), this));
}

bool RectangleCollider::CheckCollision(RectangleCollider const* other) const
{
	Vector3<float> aMin =        gameObject->getTransform()->getPosition();
	Vector3<float> bMin = other->gameObject->getTransform()->getPosition();
	Vector3<float> aMax = aMin + Vector3<float>(       w,        h, 0);
	Vector3<float> bMax = bMin + Vector3<float>(other->w, other->h, 0);

	Vector3<float> overlapMinCorner(
		SDL_max(aMin.getX(), bMin.getX()),
		SDL_max(aMin.getY(), bMin.getY()),
		SDL_max(aMin.getZ(), bMin.getZ())
	);

	Vector3<float> overlapMaxCorner(
		SDL_min(aMax.getX(), bMax.getX()),
		SDL_min(aMax.getY(), bMax.getY()),
		SDL_min(aMax.getZ(), bMax.getZ())
	);

	return overlapMinCorner.getX() <= overlapMaxCorner.getX()
		&& overlapMinCorner.getY() <= overlapMaxCorner.getY();
		//&& overlapMinCorner.getZ() <= overlapMaxCorner.getZ();
}

int RectangleCollider::GetCollisions(RectangleCollider** outArr) const
{
	int nHits = 0;
	for (RectangleCollider* i : RectangleCollider::REGISTRY) if (i != this && CheckCollision(i)) {
		if (outArr) outArr[nHits] = i;
		++nHits;
	}
	return nHits;
}
