#pragma once

#include <functional>

#include "Vector2.inl"
#include "Alignment.hpp"
#include "game/Transform.hpp"

class Renderer;

class GUIElement
{
	AnchoredPosition minCorner;
	AnchoredPosition maxCorner;
protected:
	Vector2f getCenter() const;
	Vector2f getSize() const;

public:
	GUIElement(const Vector2f& relativePosition, const Vector2f& size);
	virtual ~GUIElement() {}

	virtual void draw(Renderer* dest) const = 0;

	virtual bool intersects(const Vector2f& pos) const;

	void setActive(const bool& active);
	inline bool isActive() const { return mIsActive; }
	bool isActiveInHierarchy() const;

	//For use by Containers. Frontend programmer should never touch this.
	inline void setParent(GUIElement* newParent) { parent = newParent; }
	inline GUIElement* getParent() { return parent; }

	//Visitor pattern
	virtual void visit(const std::function<void(GUIElement*)>& visitor);

protected:
	GUIElement* parent;
	bool mIsActive;
};
