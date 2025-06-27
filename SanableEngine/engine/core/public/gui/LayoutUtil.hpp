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
	private: // Use setters for safety
		float minSize = 0; //Satisfied first
		float preferredSize = 0; //Satisfied second, in a 1:1 ratio
		float maxSize = INFINITY; //Satisfied last, in accordance with flexWeight
	public:
		float flexWeight = 1;

		constexpr LinearElementView& setMinSize(float val);
		constexpr LinearElementView& setPreferredSize(float val);
		constexpr LinearElementView& setMaxSize(float val);
		constexpr LinearElementView& setFixedSize(float val);

		constexpr inline float getMinSize() const { return minSize; }
		constexpr inline float getPreferredSize() const { return preferredSize; }
		constexpr inline float getMaxSize() const { return maxSize; }
	};

	ENGINEGUI_API void linear(float val_min, float val_max, size_t count, const LinearElementView* elementViews, float* locs_out, float* sizes_out);
	ENGINEGUI_API void linearCalcSpace(size_t count, const LinearElementView* elementViews, float* minSize_out, float* preferredSize_out, float* maxSize_out, float* flexWeight_out);



	//
	// Inline funcs
	//

	inline constexpr LinearElementView& LinearElementView::setMinSize(float val)
	{
		minSize = val;
		preferredSize = std::max(preferredSize, val);
		maxSize = std::max(maxSize, val);
		return *this;
	}

	inline constexpr LinearElementView& LinearElementView::setPreferredSize(float val)
	{
		minSize = std::min(minSize, val);
		preferredSize = val;
		maxSize = std::max(maxSize, val);
		return *this;
	}

	inline constexpr LinearElementView& LinearElementView::setMaxSize(float val)
	{
		minSize = std::min(minSize, val);
		preferredSize = std::min(preferredSize, val);
		maxSize = val;
		return *this;
	}

	inline constexpr LinearElementView& LinearElementView::setFixedSize(float val)
	{
		minSize = preferredSize = maxSize = val;
		flexWeight = 0;
		return *this;
	}
}
