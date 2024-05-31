#include "gui/LayoutUtil.hpp"

#include "gui/WidgetTransform.hpp"

float invlerp(float t, float min, float max)
{
	return (t-min)/(max-min);
}

float clamp(float t, float min, float max)
{
	if (t < min) return min;
	else if (t > max) return max;
	else return t;
}

void LayoutUtil::linear(float val_min, float val_max, size_t count, const LinearElementView* elementViews, float* locs_out, float* sizes_out)
{
	float totalSpace = val_max-val_min;

	//Calc space requirements
	float totalMinSize = 0;
	float totalPreferredSize = 0;
	float totalMaxSize = 0;
	float totalFlexWeight = 0;
	for (size_t i = 0; i < count; ++i)
	{
		totalMinSize       += elementViews[i].minSize;
		totalPreferredSize += elementViews[i].preferredSize;
		totalMaxSize       += elementViews[i].maxSize;
		totalFlexWeight    += elementViews[i].flexWeight;
	}

	float preferredAvailableRatio = clamp(invlerp(totalSpace, totalMinSize, totalPreferredSize), 0, 1);
	float maxFlexSpaceAvailable   = std::max(0.0f, totalSpace-totalPreferredSize); //Flat pixel size

	//Write values
	float cursor = val_min;
	for (size_t i = 0; i < count; ++i)
	{
		float elementSize = elementViews[i].minSize 
			         + (elementViews[i].preferredSize-elementViews[i].minSize) * preferredAvailableRatio
			         + maxFlexSpaceAvailable * (elementViews[i].flexWeight/totalFlexWeight);

		if (locs_out) locs_out[i] = cursor;
		if (sizes_out) sizes_out[i] = elementSize;

		cursor += elementSize;
	}
}
