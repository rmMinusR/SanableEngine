#pragma once

#include <functional>

#include "UISprite.hpp"
#include "LayoutUtil.hpp"
#include "RadioButtonWidget.hpp"

class RadioButtonWidget;
class LinearLayoutGroupWidget;

class RadioButtonGroup : public Widget
{
public:
	ENGINEGUI_API RadioButtonGroup(HUD* hud, const Material* btnMat, RadioButtonWidget::SpriteSet sprites, LayoutUtil::LinearElementView btnSizeSettings, bool vertical);
	ENGINEGUI_API ~RadioButtonGroup();

	ENGINEGUI_API virtual const Material* getMaterial() const override;
	ENGINEGUI_API virtual void renderImmediate(Renderer* renderer) override;

	ENGINEGUI_API const Material* getButtonMaterial() const;
	ENGINEGUI_API void setButtonMaterial(const Material* mat);

	ENGINEGUI_API RadioButtonWidget::SpriteSet getSprites() const;
	ENGINEGUI_API void setSprites(RadioButtonWidget::SpriteSet sprites); // Note: overrides custom sprites, if any are set

	ENGINEGUI_API void select(size_t newSelectionIndex);
	ENGINEGUI_API size_t getSelectionIndex() const;
	ENGINEGUI_API size_t getOptionsCount() const;
	ENGINEGUI_API void setCallback(std::function<void(size_t, size_t)> callback); // old, new

	ENGINEGUI_API RadioButtonWidget* addItem();
	ENGINEGUI_API void addItem(RadioButtonWidget* btn);
	ENGINEGUI_API RadioButtonWidget* insertItem(size_t index);
	ENGINEGUI_API void insertItem(RadioButtonWidget* btn, size_t index); // Note: New button keeps its sprites
	ENGINEGUI_API void detachItem(RadioButtonWidget* btn); // If btn is actively focused, tab to the right gets focus priority
	ENGINEGUI_API void removeItem(RadioButtonWidget* btn);

private:
	RadioButtonWidget::SpriteSet sprites;
	const Material* btnMat;
	LayoutUtil::LinearElementView btnSizeSettings;
	LinearLayoutGroupWidget* contentArea;

	size_t curSelection = 0;
	std::function<void(size_t, size_t)> callback;
};
