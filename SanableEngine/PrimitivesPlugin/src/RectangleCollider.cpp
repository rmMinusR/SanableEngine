#include "RectangleCollider.hpp"

#include "Vector3.inl"
#include "GameObject.hpp"

#undef min
#undef max

RectangleCollider::RectangleCollider(float w, float h) :
	Component(),
	w(w),
	h(h)
{
}

RectangleCollider::~RectangleCollider()
{
}

bool RectangleCollider::CheckCollision(RectangleCollider const* other) const
{
	Vector3<float> aMin =        getGameObject()->getTransform()->getPosition();
	Vector3<float> bMin = other->getGameObject()->getTransform()->getPosition();
	Vector3<float> aMax = aMin + Vector3<float>(       w,        h, 0);
	Vector3<float> bMax = bMin + Vector3<float>(other->w, other->h, 0);

	Vector3<float> overlapMinCorner(
		std::max(aMin.getX(), bMin.getX()),
		std::max(aMin.getY(), bMin.getY()),
		std::max(aMin.getZ(), bMin.getZ())
	);

	Vector3<float> overlapMaxCorner(
		std::min(aMax.getX(), bMax.getX()),
		std::min(aMax.getY(), bMax.getY()),
		std::min(aMax.getZ(), bMax.getZ())
	);

	return overlapMinCorner.getX() <= overlapMaxCorner.getX()
		&& overlapMinCorner.getY() <= overlapMaxCorner.getY();
		//&& overlapMinCorner.getZ() <= overlapMaxCorner.getZ();
}

bool RectangleCollider::CheckCollisionAny() const
{
	TypedMemoryPool<RectangleCollider>* pool = getEngine()->getMemoryManager()->getSpecificPool<RectangleCollider>(false);
	for (auto it = ((RawMemoryPool*)pool)->cbegin(); it != ((RawMemoryPool*)pool)->cend(); ++it) if (*it != this && CheckCollision((RectangleCollider*)*it)) return true;
	return false;
}
