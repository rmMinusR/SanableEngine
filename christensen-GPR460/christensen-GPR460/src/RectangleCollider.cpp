#include "RectangleCollider.hpp"

#include "Vector3.inl"
#include "GameObject.hpp"

std::vector<RectangleCollider*> RectangleCollider::REGISTRY;

RectangleCollider::RectangleCollider(float w, float h) :
	Component(),
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
	Vector3<float> aMin =        getGameObject()->getTransform()->getPosition();
	Vector3<float> bMin = other->getGameObject()->getTransform()->getPosition();
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

bool RectangleCollider::CheckCollisionAny() const
{
	for (RectangleCollider* i : RectangleCollider::REGISTRY) if (i != this && CheckCollision(i)) return true;
	return false;
}
