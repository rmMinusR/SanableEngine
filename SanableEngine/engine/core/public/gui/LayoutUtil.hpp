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
	
	namespace Stretch
	{
		//Writes to WidgetTransforms
		ENGINEGUI_API void vertical  (UIRect container, const std::vector<std::pair<WidgetTransform*, float>>& widgets, Padding padding);
		ENGINEGUI_API void horizontal(UIRect container, const std::vector<std::pair<WidgetTransform*, float>>& widgets, Padding padding);

		ENGINEGUI_API std::vector<UIRect> vertical  (UIRect container, const std::vector<float>& weights);
		ENGINEGUI_API std::vector<UIRect> horizontal(UIRect container, const std::vector<float>& weights);

		ENGINEGUI_API std::vector<UIRect> vertical  (UIRect container, const std::vector<float>& weights, Padding padding);
		ENGINEGUI_API std::vector<UIRect> horizontal(UIRect container, const std::vector<float>& weights, Padding padding);
	}
}
