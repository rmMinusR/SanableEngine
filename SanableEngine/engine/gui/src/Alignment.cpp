#include "Alignment.hpp"

Vector2f Alignment::transform(const Vector2f& point, const Vector2f& containerSize, const Alignment& alignFrom, const Alignment& alignTo)
{
	return point + containerSize * (alignTo.v - alignFrom.v);
}

Vector2f AnchoredPosition::evaluate(const Rect<float>& containerRect) const
{
	return containerRect.position + containerRect.size*alignment.v + relPosition;
}
