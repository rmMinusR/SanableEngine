#pragma once

#include <vector>

#include "Vector2.inl"

#include "GUIElement.hpp"

class GUIContainer : public GUIElement
{
public:
	GUIContainer(const Vector2f& relativePosition, const Vector2f& size);
	GUIContainer(const Vector2f& relativePosition, const Vector2f& size, std::initializer_list<GUIElement*> children);
	virtual ~GUIContainer();

	void addChild(GUIElement* child); //NOTE: GUIContainer takes ownership of children
	inline void addChildren(std::initializer_list<GUIElement*> children)  { for (GUIElement* i : children) addChild(i); } //Syntax sugar
	void removeAllChildren();

	virtual void draw(Renderer* renderer, Rect<float> workingArea) const override;

	virtual void visit(const std::function<void(GUIElement*)>& visitor) override;

protected:
	std::vector<GUIElement*> children;
};
