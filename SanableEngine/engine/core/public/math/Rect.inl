#pragma once

#include "math/Vector2.inl"

template<typename T>
struct Rect
{
	Vector2<T> topLeft;
	Vector2<T> size;
	
	const Vector2<T> center() const { return topLeft+size/2; }
	const Vector2<T> bottomRight() const { return topLeft+size; }

	bool contains(Vector2<T> pos) const
	{
		return topLeft.x <= pos.x && pos.x <= topLeft.x+size.x
			&& topLeft.y <= pos.y && pos.y <= topLeft.y+size.y;
	}


	static Rect<T> fromMinMax(Vector2<T> min, Vector2<T> max)
	{
		Rect<T> out;
		out.topLeft = min;
		out.size = max - min;
		return out;
	}

	static Rect<T> fromCenterExtents(Vector2<T> center, Vector2<T> extents)
	{
		Rect<T> out;
		out.topLeft = center-extents;
		out.size    = center+extents;
		return out;
	}
};
