#include "RectangleCollider.hpp"

#include "Vector3.inl"
#include "game/Game.hpp"
#include "game/GameObject.hpp"

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
	Vector3f aCenter = getGameObject()->getTransform()->getPosition();
	Vector3f aSize = Vector3<float>(w, h, 0);
	Vector3f bCenter = other->getGameObject()->getTransform()->getPosition();
	Vector3f bSize = Vector3<float>(other->w, other->h, 0);

	Vector3f aMin = aCenter - aSize/2.0f;
	Vector3f aMax = aCenter + aSize/2.0f;
	Vector3f bMin = bCenter - bSize/2.0f;
	Vector3f bMax = bCenter + bSize/2.0f;

	Vector3<float> overlapMinCorner(
		std::max(aMin.x, bMin.x),
		std::max(aMin.y, bMin.y),
		std::max(aMin.z, bMin.z)
	);

	Vector3<float> overlapMaxCorner(
		std::min(aMax.x, bMax.x),
		std::min(aMax.y, bMax.y),
		std::min(aMax.z, bMax.z)
	);

	return overlapMinCorner.x <= overlapMaxCorner.x
		&& overlapMinCorner.y <= overlapMaxCorner.y;
		//&& overlapMinCorner.z <= overlapMaxCorner.z;
}

bool RectangleCollider::CheckCollisionAny() const
{
	TypedMemoryPool<RectangleCollider>* pool = getEngine()->getApplication()->getMemoryManager()->getSpecificPool<RectangleCollider>(false);
	for (auto it = ((RawMemoryPool*)pool)->cbegin(); it != ((RawMemoryPool*)pool)->cend(); ++it)
	{
		if (*it != this && CheckCollision((RectangleCollider*)*it)) return true;
	}
	return false;
}

int RectangleCollider::GetCollisions(RectangleCollider** outArr) const
{
	int nHits = 0;
	TypedMemoryPool<RectangleCollider>* pool = getEngine()->getApplication()->getMemoryManager()->getSpecificPool<RectangleCollider>(false);
	for (auto it = pool->cbegin(); it != pool->cend(); ++it)
	{
		RectangleCollider* c = (RectangleCollider*)*it;

		if (c != this && CheckCollision(c))
		{
			if (outArr) outArr[nHits] = c; //Array it optional, only write if it is present
			++nHits;
		}
	}
	return nHits;
}
