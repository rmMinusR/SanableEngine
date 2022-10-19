#include "RectangleCollider.hpp"

RectangleCollider::RectangleCollider(GameObject& owner, float w, float h) :
	Component(owner),
	w(w),
	h(h)
{
}

RectangleCollider::~RectangleCollider()
{
}
