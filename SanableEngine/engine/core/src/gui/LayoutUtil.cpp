#include "gui/LayoutUtil.hpp"

#include <algorithm>
#include "math/Linear.inl"
#include "gui/WidgetTransform.hpp"

void LayoutUtil::linear(float val_min, float val_max, size_t count, const LinearElementView* elementViews, float* locs_out, float* sizes_out)
{
	float totalSpace = val_max-val_min;

	//Calc space requirements
	float totalMinSize, totalPreferredSize, totalMaxSize, totalFlexWeight;
	linearCalcSpace(count, elementViews, &totalMinSize, &totalPreferredSize, &totalMaxSize, &totalFlexWeight);

	float preferredAvailableRatio = std::clamp<float>(invlerp(totalSpace, totalMinSize, totalPreferredSize), 0, 1);
	float flexSpaceAvailable      = std::max(0.0f, totalSpace-totalPreferredSize); //Flat pixel size

	// Write values - check upper size limit first
	bool alreadyMaxSize[count];
	memset(alreadyMaxSize, 0, sizeof(alreadyMaxSize));
	for (size_t i = 0; i < count; ++i)
	{
		float flexCapacity = elementViews[i].getMaxSize() - elementViews[i].getPreferredSize();
		float flexWeightShare = totalFlexWeight > 0 ? elementViews[i].flexWeight / totalFlexWeight : 0;
		float flexSpaceForMaxSize = flexWeightShare > 0 ? flexCapacity / flexWeightShare : 0;
		if (flexSpaceAvailable > flexSpaceForMaxSize)
		{
			flexSpaceAvailable -= flexCapacity;
			totalFlexWeight -= elementViews[i].flexWeight;
			alreadyMaxSize[i] = true;
		}
	}

	//Write values - remaining flex elements
	float cursor = val_min;
	for (size_t i = 0; i < count; ++i)
	{
		float elementSize;
		if (!alreadyMaxSize[i])
		{
			elementSize = elementViews[i].getMinSize()
						 + (elementViews[i].getPreferredSize()-elementViews[i].getMinSize()) * preferredAvailableRatio;
			if (totalFlexWeight > 0) elementSize += flexSpaceAvailable * (elementViews[i].flexWeight/totalFlexWeight);
		}
		else elementSize = elementViews[i].getMaxSize();

		if (locs_out) locs_out[i] = cursor;
		if (sizes_out) sizes_out[i] = elementSize;
		
		cursor += elementSize;
	}
}

void LayoutUtil::linearCalcSpace(size_t count, const LinearElementView* elementViews, float* minSize_out, float* preferredSize_out, float* maxSize_out, float* flexWeight_out)
{
	*minSize_out       = 0;
	*preferredSize_out = 0;
	*maxSize_out       = 0;
	*flexWeight_out    = 0;

	for (size_t i = 0; i < count; ++i)
	{
		*minSize_out       += elementViews[i].getMinSize();
		*preferredSize_out += elementViews[i].getPreferredSize();
		*maxSize_out       += elementViews[i].getMaxSize();
		*flexWeight_out    += elementViews[i].flexWeight;
	}
}
