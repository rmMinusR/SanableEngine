#include "gui/LayoutUtil.hpp"

#include "gui/WidgetTransform.hpp"

void LayoutUtil::Stretch::vertical(UIRect container, const std::vector<std::pair<WidgetTransform*, float>>& widgets, Padding padding)
{
	//Retrieve only weights
	std::vector<float> weights;
	weights.reserve(widgets.size());
	for (const auto& p : widgets) weights.push_back(p.second);

	//Defer
	std::vector<UIRect> rects = vertical(container, weights, padding);

	//Apply
	for (int i = 0; i < rects.size(); ++i) widgets[i].first->setRectByOffsets(rects[i]);
}

void LayoutUtil::Stretch::horizontal(UIRect container, const std::vector<std::pair<WidgetTransform*, float>>& widgets, Padding padding)
{
	//Retrieve only weights
	std::vector<float> weights;
	weights.reserve(widgets.size());
	for (const auto& p : widgets) weights.push_back(p.second);

	//Defer
	std::vector<UIRect> rects = horizontal(container, weights, padding);

	//Apply
	for (int i = 0; i < rects.size(); ++i) widgets[i].first->setRectByOffsets(rects[i]);
}

std::vector<LayoutUtil::UIRect> LayoutUtil::Stretch::vertical(UIRect container, const std::vector<float>& weights)
{
	float weightSum = 0;
	for (float w : weights) weightSum += w;

	Vector2f cursor = container.topLeft;

	std::vector<UIRect> out;
	out.reserve(weights.size());
	for (float w : weights)
	{
		float elementHeight = container.size.y * (w/weightSum);
		out.push_back( UIRect::fromMinMax(cursor, cursor+Vector2f(container.size.x, elementHeight)) ); //Output rect
		cursor.y += elementHeight; //Advance cursor
	}

	return out;
}

std::vector<LayoutUtil::UIRect> LayoutUtil::Stretch::horizontal(UIRect container, const std::vector<float>& weights)
{
	float weightSum = 0;
	for (float w : weights) weightSum += w;

	Vector2f cursor = container.topLeft;

	std::vector<UIRect> out;
	out.reserve(weights.size());
	for (float w : weights)
	{
		float elementWidth = container.size.x * (w/weightSum);
		out.push_back( UIRect::fromMinMax(cursor, cursor+Vector2f(elementWidth, container.size.y)) ); //Output rect
		cursor.x += elementWidth; //Advance cursor
	}

	return out;
}

std::vector<LayoutUtil::UIRect> LayoutUtil::Stretch::vertical(UIRect container, const std::vector<float>& weights, Padding padding)
{
	//Calc inner rect
	UIRect containerWithOuterPadding = UIRect::fromMinMax(
		container.topLeft       + Vector2f(padding.left , padding.top   ),
		container.bottomRight()	- Vector2f(padding.right, padding.bottom)
	);

	//Defer
	std::vector<LayoutUtil::UIRect> out = vertical(
		UIRect::fromMinMax(
			containerWithOuterPadding.topLeft,
			containerWithOuterPadding.bottomRight() - Vector2f(0, (weights.size()-1)*padding.betweenElements)
		),
		weights
	);

	//Adjust for padding between elements
	for (int i = 0; i < out.size(); ++i) out[i].topLeft.y += i*padding.betweenElements;

	//Done
	return out;
}

std::vector<LayoutUtil::UIRect> LayoutUtil::Stretch::horizontal(UIRect container, const std::vector<float>& weights, Padding padding)
{
	//Calc inner rect
	UIRect containerWithOuterPadding = UIRect::fromMinMax(
		container.topLeft       + Vector2f(padding.left , padding.top   ),
		container.bottomRight()	- Vector2f(padding.right, padding.bottom)
	);

	//Defer
	std::vector<LayoutUtil::UIRect> out = horizontal(
		UIRect::fromMinMax(
			containerWithOuterPadding.topLeft,
			containerWithOuterPadding.bottomRight() - Vector2f((weights.size()-1)*padding.betweenElements, 0)
		),
		weights
	);

	//Adjust for padding between elements
	for (int i = 0; i < out.size(); ++i) out[i].topLeft.x += i*padding.betweenElements;

	//Done
	return out;
}
