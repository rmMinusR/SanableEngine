#pragma once

#include "gui/Widget.hpp"

struct TypeInfo;
struct FieldInfo;
class UISprite;
class Material;
class Font;

class TypeInfoView : public Widget
{
	const TypeInfo* target;
	Vector2f byteSize = Vector2f(36, 36);

	const Material* imageMat;
	const UISprite* fieldSprite;
	const UISprite* parentSprite;
	const Material* textMat;
	const Font* textFont;

	constexpr static size_t bytesPerColumn = 8;

	void refresh();
public:
	TypeInfoView(HUD* hud, const TypeInfo* target, const Material* imageMat, const UISprite* fieldSprite, const UISprite* parentSprite, const Material* textMat, const Font* textFont);
	~TypeInfoView();
	
	virtual const Material* getMaterial() const override;
	virtual void renderImmediate(Renderer* target) override;
};
