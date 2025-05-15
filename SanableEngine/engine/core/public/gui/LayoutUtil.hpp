#pragma once

#include <vector>

#include "dllapi.h"

#include "math/Vector2.inl"
#include "math/Rect.inl"

struct WidgetTransform;

namespace LayoutUtil
{
	typedef Rect<float> UIRect;

	struct Padding
	{
		float top = 0, left = 0, bottom = 0, right = 0;
		float betweenElements = 0;
	};
	
	struct LinearElementView
	{
		float minSize; //Satisfied first
		float preferredSize; //Satisfied second, in a 1:1 ratio
		float maxSize; //Satisfied last, in accordance with flexWeight
		float flexWeight;
	};
	ENGINEGUI_API void linear(float val_min, float val_max, size_t count, const LinearElementView* elementViews, float* locs_out, float* sizes_out);
	ENGINEGUI_API void linearCalcSpace(size_t count, const LinearElementView* elementViews, float* minSize_out, float* preferredSize_out, float* maxSize_out, float* flexWeight_out);
}
