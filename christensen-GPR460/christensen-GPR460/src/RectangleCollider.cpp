#include "RectangleCollider.hpp"

#include "Vector3.inl"
#include "GameObject.hpp"

RectangleCollider::RectangleCollider(GameObject& owner, float w, float h) :
	Component(owner),
	w(w),
	h(h)
{
}

RectangleCollider::~RectangleCollider()
{
}

bool RectangleCollider::CheckCollision(RectangleCollider const* other) const
{
	Vector3<float> aPos =        gameObject->getTransform()->getPosition();
	Vector3<float> bPos = other->gameObject->getTransform()->getPosition();

	Vector3<float> overlapMinCorner(
		SDL_max(aPos.getX(), bPos.getX()),
		SDL_max(aPos.getY(), bPos.getY()),
		SDL_max(aPos.getZ(), bPos.getZ())
	);

	Vector3<float> overlapMaxCorner(
		SDL_min(aPos.getX()+w, bPos.getX()+other->w),
		SDL_min(aPos.getY()+h, bPos.getY()+other->h),
		SDL_min(aPos.getZ()  , bPos.getZ()         )
	);

	return overlapMinCorner.getX() < overlapMaxCorner.getX()
		&& overlapMinCorner.getY() < overlapMaxCorner.getY()
		&& overlapMinCorner.getZ() < overlapMaxCorner.getZ();
}
