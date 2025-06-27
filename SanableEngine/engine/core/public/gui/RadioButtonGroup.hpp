#pragma once

#include <functional>

#include "Widget.hpp"
#include "UISprite.hpp"
#include "LayoutUtil.hpp"

class ButtonWidget;
class LinearLayoutGroupWidget;

class RadioButtonGroup : public Widget
{
public:
	struct SpriteSet
	{
		const UISprite* normal;
		const UISprite* pressed;
		const UISprite* selected;
	};

	ENGINEGUI_API RadioButtonGroup(HUD* hud, const Material* btnMat, SpriteSet sprites, LayoutUtil::LinearElementView btnSizeSettings, bool vertical);
	ENGINEGUI_API ~RadioButtonGroup();

	ENGINEGUI_API virtual const Material* getMaterial() const override;
	ENGINEGUI_API virtual void renderImmediate(Renderer* renderer) override;

	ENGINEGUI_API void select(size_t newSelectionIndex);
	ENGINEGUI_API size_t getSelectionIndex() const;
	ENGINEGUI_API void setCallback(std::function<void(size_t, size_t)> callback); // old, new

	ENGINEGUI_API ButtonWidget* addItem();

private:
	SpriteSet sprites;
	const Material* btnMat;
	LayoutUtil::LinearElementView btnSizeSettings;
	LinearLayoutGroupWidget* contentArea;

	size_t curSelection = 0;
	std::function<void(size_t, size_t)> callback;
};
