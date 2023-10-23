#pragma once

#include "dllapi.h"
#include "Rect.hpp"

struct Alignment
{
	union
	{
		struct { float x, y; };
		Vector2<float> v;
	};

	ENGINEGUI_API static Vector2f transform(const Vector2f& point, const Vector2f& containerSize, const Alignment& alignFrom, const Alignment& alignTo);
};

struct AnchoredPosition
{
	Alignment alignment;
	Vector2<float> relPosition;

	ENGINEGUI_API Vector2f evaluate(const Rect<float>& containerRect) const;
};
