#pragma once

#include "Vector2.inl"

template<typename T>
struct Rect
{
	union
	{
		struct { T x, y; };
		Vector2<T> position;
	};

	union
	{
		struct { T width, height; };
		Vector2<T> size;
	};
};
